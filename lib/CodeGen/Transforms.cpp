#include "yac/CodeGen/Transforms.h"
#include <iostream>
#include <map>
#include <queue>
#include <stack>

namespace yac {

// ===----------------------------------------------------------------------===
// Mem2Reg Pass
// ===----------------------------------------------------------------------===

bool Mem2RegPass::run(IRFunction* F, AnalysisManager& AM) {
  CurrentFunc = F;
  DT = &AM.get<DominatorTree>();

  std::vector<AllocaInfo> Allocas;

  // Identify promotable allocas
  identifyPromotableAllocas(Allocas);

  if (Allocas.empty()) {
    return false; // No changes
  }

  bool Changed = false;

  // Collect all instructions to remove (defer removal until end)
  std::set<IRInstruction*> ToRemove;

  for (AllocaInfo& Info : Allocas) {
    if (!Info.IsPromotable) continue;

    // Compute which blocks have stores
    computeDefBlocks(Info);

    // Insert phi nodes
    insertPhiNodes(Info);

    // Rename variables (SSA construction)
    renameVariables(Info);

    // Replace loads with SSA values
    replaceLoadsAndRemoveStores(Info);

    // Mark instructions for removal (but don't remove yet!)
    for (auto* Load : Info.Uses) {
      ToRemove.insert(Load);
    }
    for (auto* Store : Info.DefiningStores) {
      ToRemove.insert(Store);
    }
    ToRemove.insert(Info.Alloca);

    Changed = true;
  }

  // Now remove all dead instructions at once
  for (auto& BB : CurrentFunc->getBlocks()) {
    auto& Insts = const_cast<std::vector<std::unique_ptr<IRInstruction>>&>(
        BB->getInstructions());

    auto it = Insts.begin();
    while (it != Insts.end()) {
      if (ToRemove.count(it->get())) {
        it = Insts.erase(it);
      } else {
        ++it;
      }
    }
  }

  return Changed;
}

void Mem2RegPass::identifyPromotableAllocas(
    std::vector<AllocaInfo>& Allocas) {

  if (CurrentFunc->getBlocks().empty()) return;

  // Look at entry block for allocas
  IRBasicBlock* Entry = CurrentFunc->getBlocks()[0].get();

  for (auto& Inst : Entry->getInstructions()) {
    auto* Alloca = dynamic_cast<IRAllocaInst*>(Inst.get());
    if (!Alloca) continue;

    AllocaInfo Info;
    Info.Alloca = Alloca;

    // Find all loads and stores
    for (const auto& BB : CurrentFunc->getBlocks()) {
      for (const auto& I : BB->getInstructions()) {
        // Check if this instruction uses the alloca
        if (auto* Store = dynamic_cast<IRStoreInst*>(I.get())) {
          if (Store->getPtr() == Alloca->getResult()) {
            Info.DefiningStores.push_back(Store);
          }
        } else if (auto* Load = dynamic_cast<IRLoadInst*>(I.get())) {
          if (Load->getPtr() == Alloca->getResult()) {
            Info.Uses.push_back(Load);
          }
        }
      }
    }

    // For now, mark all as promotable
    // TODO: Check for address-taken, non-scalar types
    Info.IsPromotable = true;

    Allocas.push_back(Info);
  }
}

void Mem2RegPass::computeDefBlocks(AllocaInfo& Info) {
  Info.DefBlocks.clear();

  for (IRStoreInst* Store : Info.DefiningStores) {
    IRBasicBlock* BB = Store->getParent();
    if (BB) {
      Info.DefBlocks.insert(BB);
    }
  }
}

std::set<IRBasicBlock*> Mem2RegPass::computeDominanceFrontier(
    const std::set<IRBasicBlock*>& Blocks) {

  std::set<IRBasicBlock*> Frontier;

  // For each block in the set
  for (IRBasicBlock* BB : Blocks) {
    // For each successor
    for (IRBasicBlock* Succ : BB->getSuccessors()) {
      // If BB doesn't dominate Succ, Succ is in the dominance frontier
      if (!DT->dominates(BB, Succ)) {
        Frontier.insert(Succ);
      }
    }
  }

  return Frontier;
}

void Mem2RegPass::insertPhiNodes(AllocaInfo& Info) {
  // Compute dominance frontier of all def blocks
  std::set<IRBasicBlock*> PhiBlocks;
  std::queue<IRBasicBlock*> Worklist;

  // Initialize worklist with def blocks
  for (IRBasicBlock* BB : Info.DefBlocks) {
    Worklist.push(BB);
  }

  std::set<IRBasicBlock*> Visited = Info.DefBlocks;

  // Iterate to find all blocks that need phi nodes
  while (!Worklist.empty()) {
    IRBasicBlock* BB = Worklist.front();
    Worklist.pop();

    // Compute dominance frontier of this block
    std::set<IRBasicBlock*> SingleBlock = {BB};
    std::set<IRBasicBlock*> DF = computeDominanceFrontier(SingleBlock);

    for (IRBasicBlock* FrontierBlock : DF) {
      if (PhiBlocks.count(FrontierBlock)) continue;

      // Insert phi node at the beginning of this block
      PhiBlocks.insert(FrontierBlock);

      // Create phi instruction
      IRValue* PhiResult = CurrentFunc->createValue(
          IRValue::VK_Temp,
          "phi_" + Info.Alloca->getResult()->getName() + "_" + std::to_string(PhiBlocks.size()),
          Info.Alloca->getAllocType());

      auto PhiPtr = std::make_unique<IRPhiInst>(PhiResult);
      IRPhiInst* Phi = PhiPtr.get();

      // Store the phi node for this alloca in this block
      Info.PhiNodes[FrontierBlock] = Phi;

      // We'll add incoming values during renaming
      // For now, just insert the phi at the start
      auto& Insts = const_cast<std::vector<std::unique_ptr<IRInstruction>>&>(
          FrontierBlock->getInstructions());

      // Find first non-phi instruction
      size_t InsertPos = 0;
      for (size_t i = 0; i < Insts.size(); ++i) {
        if (!dynamic_cast<IRPhiInst*>(Insts[i].get())) {
          InsertPos = i;
          break;
        }
      }

      PhiPtr->setParent(FrontierBlock);
      Insts.insert(Insts.begin() + InsertPos, std::move(PhiPtr));

      // Add to worklist if not visited
      if (!Visited.count(FrontierBlock)) {
        Visited.insert(FrontierBlock);
        Worklist.push(FrontierBlock);
      }
    }
  }
}

void Mem2RegPass::renameVariables(AllocaInfo& Info) {
  // SSA renaming using a stack-based approach
  // CurrentDef maps each block to the current SSA value
  std::map<IRBasicBlock*, IRValue*> CurrentDef;
  std::set<IRBasicBlock*> Visited;

  // Start from entry block
  if (CurrentFunc->getBlocks().empty()) return;
  IRBasicBlock* Entry = CurrentFunc->getBlocks()[0].get();

  renameInBlock(Info, Entry, CurrentDef, Visited);
}

void Mem2RegPass::renameInBlock(
    AllocaInfo& Info,
    IRBasicBlock* BB,
    std::map<IRBasicBlock*, IRValue*>& CurrentDef,
    std::set<IRBasicBlock*>& Visited) {

  if (Visited.count(BB)) return;
  Visited.insert(BB);

  IRValue* IncomingValue = nullptr;

  // Check if this block has a phi node for this alloca
  auto PhiIt = Info.PhiNodes.find(BB);
  if (PhiIt != Info.PhiNodes.end()) {
    IncomingValue = PhiIt->second->getResult();
    CurrentDef[BB] = IncomingValue;  // Record phi as current definition
  } else {
    // If no phi, inherit value from dominating block
    // Find the immediate dominator's value
    if (auto* Node = DT->getNode(BB)) {
      if (auto* IDom = Node->IDom) {
        IRBasicBlock* DomBB = IDom->Block;
        if (CurrentDef.count(DomBB)) {
          IncomingValue = CurrentDef[DomBB];
          // Propagate the value so children can inherit it
          CurrentDef[BB] = IncomingValue;
        }
      }
    }
  }

  // Process instructions in this block
  for (const auto& Inst : BB->getInstructions()) {
    // Handle stores: update current definition
    if (auto* Store = dynamic_cast<IRStoreInst*>(Inst.get())) {
      if (Store->getPtr() == Info.Alloca->getResult()) {
        IncomingValue = Store->getValue();
        CurrentDef[BB] = IncomingValue;
      }
    }
    // Handle loads: record replacement with current value
    else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst.get())) {
      if (Load->getPtr() == Info.Alloca->getResult()) {
        if (IncomingValue) {
          // Record that this load's result should be replaced
          Info.Replacements[Load->getResult()] = IncomingValue;
        }
      }
    }
  }

  // Recursively visit dominated children FIRST
  if (auto* Node = DT->getNode(BB)) {
    for (auto* Child : Node->Children) {
      renameInBlock(Info, Child->Block, CurrentDef, Visited);
    }
  }

  // AFTER visiting children, fill in phi nodes for ALL successors
  // This handles both forward edges and backedges (loops)
  for (IRBasicBlock* Succ : BB->getSuccessors()) {
    // Check if successor has a phi node for this alloca
    auto SuccPhiIt = Info.PhiNodes.find(Succ);
    if (SuccPhiIt != Info.PhiNodes.end()) {
      // Determine what value is available at the end of this block
      IRValue* ValueAtEnd = CurrentDef.count(BB) ? CurrentDef[BB] : IncomingValue;

      if (ValueAtEnd) {
        // Add incoming value from this block to the phi for this alloca
        SuccPhiIt->second->addIncoming(ValueAtEnd, BB);
      }
    }
  }
}

IRValue* Mem2RegPass::createSSAValue(IRValue* OrigValue) {
  // Create a new SSA value (temp)
  return CurrentFunc->createValue(
      IRValue::VK_Temp,
      "ssa_" + OrigValue->getName(),
      OrigValue->getType());
}

void Mem2RegPass::replaceValueInInstruction(IRInstruction* Inst,
                                              IRValue* Old,
                                              IRValue* New) {
  // Replace operands in different instruction types
  if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst)) {
    if (BinOp->getLHS() == Old) {
      BinOp->setLHS(New);
    }
    if (BinOp->getRHS() == Old) {
      BinOp->setRHS(New);
    }
  }
  else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst)) {
    if (UnOp->getOperand() == Old) {
      UnOp->setOperand(New);
    }
  }
  else if (auto* Store = dynamic_cast<IRStoreInst*>(Inst)) {
    if (Store->getValue() == Old) {
      Store->setValue(New);
    }
    if (Store->getPtr() == Old) {
      Store->setPtr(New);
    }
  }
  else if (auto* Ret = dynamic_cast<IRRetInst*>(Inst)) {
    if (Ret->hasRetValue() && Ret->getRetValue() == Old) {
      Ret->setRetValue(New);
    }
  }
  else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(Inst)) {
    if (CondBr->getCondition() == Old) {
      CondBr->setCondition(New);
    }
  }
  else if (auto* Call = dynamic_cast<IRCallInst*>(Inst)) {
    // Calls need more complex handling for args
    // For now, skip - would need setArg(index, value) method
    (void)Call;
  }
  else if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst)) {
    // Replace incoming values in phi nodes
    Phi->replaceIncomingValue(Old, New);
  }
}

void Mem2RegPass::replaceLoadsAndRemoveStores(AllocaInfo& Info) {
  // Replace uses of loaded values with SSA values
  for (auto& BB : CurrentFunc->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      // Don't replace in loads/stores/allocas themselves
      if (dynamic_cast<IRLoadInst*>(Inst.get()) ||
          dynamic_cast<IRStoreInst*>(Inst.get()) ||
          dynamic_cast<IRAllocaInst*>(Inst.get())) {
        continue;
      }

      // Replace all uses of loaded values with their SSA values
      for (const auto& Replacement : Info.Replacements) {
        replaceValueInInstruction(Inst.get(), Replacement.first, Replacement.second);
      }
    }
  }
  // Note: Instruction removal is now handled in run() after all allocas are processed
}

// ===----------------------------------------------------------------------===
// DCE Pass
// ===----------------------------------------------------------------------===

bool DCEPass::run(IRFunction* F, AnalysisManager& AM) {
  std::set<IRInstruction*> Live;

  // Collect initially live instructions (side effects)
  collectLiveInstructions(F, Live);

  // Build value-to-defining-instruction map
  std::map<IRValue*, IRInstruction*> DefMap;
  for (const auto& BB : F->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        DefMap[BinOp->getResult()] = Inst.get();
      } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
        DefMap[UnOp->getResult()] = Inst.get();
      } else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst.get())) {
        DefMap[Load->getResult()] = Inst.get();
      } else if (auto* Alloca = dynamic_cast<IRAllocaInst*>(Inst.get())) {
        DefMap[Alloca->getResult()] = Inst.get();
      } else if (auto* Call = dynamic_cast<IRCallInst*>(Inst.get())) {
        if (Call->getResult()) {
          DefMap[Call->getResult()] = Inst.get();
        }
      } else if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
        DefMap[Phi->getResult()] = Inst.get();
      }
    }
  }

  // Mark transitively live instructions
  bool Changed = true;
  while (Changed) {
    Changed = false;
    for (const auto& BB : F->getBlocks()) {
      for (const auto& Inst : BB->getInstructions()) {
        if (Live.count(Inst.get())) {
          // Extract operands and mark their defining instructions as live
          std::vector<IRValue*> Operands;

          if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
            Operands.push_back(BinOp->getLHS());
            Operands.push_back(BinOp->getRHS());
          } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
            Operands.push_back(UnOp->getOperand());
          } else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst.get())) {
            Operands.push_back(Load->getPtr());
          } else if (auto* Store = dynamic_cast<IRStoreInst*>(Inst.get())) {
            Operands.push_back(Store->getValue());
            Operands.push_back(Store->getPtr());
          } else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(Inst.get())) {
            Operands.push_back(CondBr->getCondition());
          } else if (auto* Ret = dynamic_cast<IRRetInst*>(Inst.get())) {
            if (Ret->hasRetValue()) {
              Operands.push_back(Ret->getRetValue());
            }
          } else if (auto* Call = dynamic_cast<IRCallInst*>(Inst.get())) {
            for (IRValue* Arg : Call->getArgs()) {
              Operands.push_back(Arg);
            }
          } else if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
            for (const auto& Entry : Phi->getIncomings()) {
              Operands.push_back(Entry.Value);
            }
          }

          // Mark defining instructions as live
          for (IRValue* Op : Operands) {
            if (Op && !Op->isConstant() && DefMap.count(Op)) {
              IRInstruction* DefInst = DefMap[Op];
              if (!Live.count(DefInst)) {
                Live.insert(DefInst);
                Changed = true;
              }
            }
          }
        }
      }
    }
  }

  // Remove dead instructions
  bool RemovedAny = false;
  for (auto& BB : F->getBlocks()) {
    auto& Insts = const_cast<std::vector<std::unique_ptr<IRInstruction>>&>(
        BB->getInstructions());

    auto it = Insts.begin();
    while (it != Insts.end()) {
      if (!Live.count(it->get()) && !isInstructionDead(it->get())) {
        it = Insts.erase(it);
        RemovedAny = true;
      } else {
        ++it;
      }
    }
  }

  return RemovedAny;
}

bool DCEPass::isInstructionDead(IRInstruction* I) {
  // Instructions with side effects are never dead
  return !I->isTerminator() &&
         I->getOpcode() != IRInstruction::Store &&
         I->getOpcode() != IRInstruction::Call;
}

void DCEPass::collectLiveInstructions(
    IRFunction* F,
    std::set<IRInstruction*>& Live) {

  for (const auto& BB : F->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      // Terminators, stores, and calls are always live
      if (Inst->isTerminator() ||
          Inst->getOpcode() == IRInstruction::Store ||
          Inst->getOpcode() == IRInstruction::Call) {
        Live.insert(Inst.get());
      }
    }
  }
}

// ===----------------------------------------------------------------------===
// SimplifyCFG Pass
// ===----------------------------------------------------------------------===

bool SimplifyCFGPass::run(IRFunction* F, AnalysisManager& AM) {
  bool Changed = false;

  // Remove unreachable blocks
  Changed |= removeUnreachableBlocks(F);

  // Merge blocks with single predecessor/successor
  // TODO: Implement block merging

  return Changed;
}

bool SimplifyCFGPass::removeUnreachableBlocks(IRFunction* F) {
  if (F->getBlocks().empty()) return false;

  // Find reachable blocks from entry
  std::set<IRBasicBlock*> Reachable;
  std::queue<IRBasicBlock*> Worklist;

  IRBasicBlock* Entry = F->getBlocks()[0].get();
  Reachable.insert(Entry);
  Worklist.push(Entry);

  while (!Worklist.empty()) {
    IRBasicBlock* BB = Worklist.front();
    Worklist.pop();

    for (IRBasicBlock* Succ : BB->getSuccessors()) {
      if (!Reachable.count(Succ)) {
        Reachable.insert(Succ);
        Worklist.push(Succ);
      }
    }
  }

  // Remove unreachable blocks
  auto& Blocks = const_cast<std::vector<std::unique_ptr<IRBasicBlock>>&>(
      F->getBlocks());

  auto it = Blocks.begin();
  bool Changed = false;
  while (it != Blocks.end()) {
    if (!Reachable.count(it->get())) {
      it = Blocks.erase(it);
      Changed = true;
    } else {
      ++it;
    }
  }

  return Changed;
}

// ===----------------------------------------------------------------------===
// ConstantPropagation Pass
// ===----------------------------------------------------------------------===

bool ConstantPropagationPass::isConstant(IRValue* V) {
  if (V->isConstant()) return true;
  return ConstantValues.count(V) > 0;
}

int64_t ConstantPropagationPass::getConstant(IRValue* V) {
  if (V->isConstant()) return V->getConstant();
  return ConstantValues[V];
}

IRValue* ConstantPropagationPass::tryFoldBinary(IRBinaryInst* BinOp) {
  IRValue* LHS = BinOp->getLHS();
  IRValue* RHS = BinOp->getRHS();

  if (!isConstant(LHS) || !isConstant(RHS)) {
    return nullptr;
  }

  int64_t L = getConstant(LHS);
  int64_t R = getConstant(RHS);
  int64_t Result = 0;

  switch (BinOp->getOpcode()) {
    case IRInstruction::Add: Result = L + R; break;
    case IRInstruction::Sub: Result = L - R; break;
    case IRInstruction::Mul: Result = L * R; break;
    case IRInstruction::Div:
      if (R == 0) return nullptr;
      Result = L / R;
      break;
    case IRInstruction::Mod:
      if (R == 0) return nullptr;
      Result = L % R;
      break;
    case IRInstruction::And: Result = L & R; break;
    case IRInstruction::Or:  Result = L | R; break;
    case IRInstruction::Xor: Result = L ^ R; break;
    case IRInstruction::Shl: Result = L << R; break;
    case IRInstruction::Shr: Result = L >> R; break;
    case IRInstruction::Lt:  Result = L < R ? 1 : 0; break;
    case IRInstruction::Le:  Result = L <= R ? 1 : 0; break;
    case IRInstruction::Gt:  Result = L > R ? 1 : 0; break;
    case IRInstruction::Ge:  Result = L >= R ? 1 : 0; break;
    case IRInstruction::Eq:  Result = L == R ? 1 : 0; break;
    case IRInstruction::Ne:  Result = L != R ? 1 : 0; break;
    default: return nullptr;
  }

  // Create a constant value
  return new IRValue(Result);
}

IRValue* ConstantPropagationPass::tryFoldCompare(IRInstruction* Cmp) {
  // Comparisons are handled by tryFoldBinary since they use IRBinaryInst
  return nullptr;
}

bool ConstantPropagationPass::run(IRFunction* F, AnalysisManager& AM) {
  bool Changed = false;
  ConstantValues.clear();

  // Iterate until no more changes (simple fixed-point iteration)
  bool LocalChanged = true;
  while (LocalChanged) {
    LocalChanged = false;

    for (auto& BB : F->getBlocks()) {
      for (auto& Inst : BB->getInstructions()) {
        // Try to fold binary operations
        if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
          // Skip if we already know this result is a constant
          if (ConstantValues.count(BinOp->getResult())) {
            continue;
          }

          IRValue* Folded = tryFoldBinary(BinOp);
          if (Folded) {
            // Record that the result is a constant (only if it's new!)
            ConstantValues[BinOp->getResult()] = Folded->getConstant();
            LocalChanged = true;
            Changed = true;
            delete Folded;
          }
        }
        // TODO: Handle other instruction types (calls, phi nodes, etc.)
      }
    }
  }

  // Second pass: Replace uses of constant values
  // For now, we just record them - DCE will clean up unused instructions
  // A full implementation would use a use-def chain to replace operands

  return Changed;
}

// ===----------------------------------------------------------------------===
// Copy Propagation Pass
// ===----------------------------------------------------------------------===

void CopyPropagationPass::buildCopyMap(IRFunction* F) {
  CopyMap.clear();

  // Find all move instructions and record copies
  for (const auto& BB : F->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      if (auto* Move = dynamic_cast<IRMoveInst*>(Inst.get())) {
        // Record: result is a copy of operand
        CopyMap[Move->getResult()] = Move->getOperand();
      }
    }
  }
}

IRValue* CopyPropagationPass::getOriginalValue(IRValue* V) {
  // Transitively follow copies to find the original value
  std::set<IRValue*> Visited;
  IRValue* Current = V;

  while (CopyMap.count(Current)) {
    // Prevent infinite loops in case of cycles
    if (Visited.count(Current)) {
      break;
    }
    Visited.insert(Current);
    Current = CopyMap[Current];
  }

  return Current;
}

void CopyPropagationPass::replaceValueInInstruction(IRInstruction* Inst,
                                                      IRValue* Old,
                                                      IRValue* New) {
  // Replace operands in different instruction types
  if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst)) {
    if (BinOp->getLHS() == Old) {
      BinOp->setLHS(New);
    }
    if (BinOp->getRHS() == Old) {
      BinOp->setRHS(New);
    }
  }
  else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst)) {
    if (UnOp->getOperand() == Old) {
      UnOp->setOperand(New);
    }
  }
  else if (auto* Store = dynamic_cast<IRStoreInst*>(Inst)) {
    if (Store->getValue() == Old) {
      Store->setValue(New);
    }
    if (Store->getPtr() == Old) {
      Store->setPtr(New);
    }
  }
  else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst)) {
    // Load only has a ptr, and we typically don't propagate through pointers
    (void)Load;
  }
  else if (auto* Ret = dynamic_cast<IRRetInst*>(Inst)) {
    if (Ret->hasRetValue() && Ret->getRetValue() == Old) {
      Ret->setRetValue(New);
    }
  }
  else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(Inst)) {
    if (CondBr->getCondition() == Old) {
      CondBr->setCondition(New);
    }
  }
  else if (auto* Move = dynamic_cast<IRMoveInst*>(Inst)) {
    // Don't replace the result, but replace the operand if it's a copy
    (void)Move;
  }
  // TODO: Handle Call, Phi, etc.
}

bool CopyPropagationPass::run(IRFunction* F, AnalysisManager& AM) {
  (void)AM;  // Unused

  bool Changed = false;

  // Build map of all copies in the function
  buildCopyMap(F);

  if (CopyMap.empty()) {
    return false;  // No copies to propagate
  }

  // Replace all uses of copied values with original values
  for (auto& BB : F->getBlocks()) {
    for (auto& Inst : BB->getInstructions()) {
      // Skip move instructions themselves (we'll let DCE remove them)
      if (dynamic_cast<IRMoveInst*>(Inst.get())) {
        continue;
      }

      // Check each value in the copy map
      for (const auto& Entry : CopyMap) {
        IRValue* CopiedValue = Entry.first;
        IRValue* OriginalValue = getOriginalValue(CopiedValue);

        // Only replace if we found a different original value
        if (OriginalValue != CopiedValue) {
          replaceValueInInstruction(Inst.get(), CopiedValue, OriginalValue);
          Changed = true;
        }
      }
    }
  }

  return Changed;
}

} // namespace yac
