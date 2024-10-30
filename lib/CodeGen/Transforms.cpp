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

// ===----------------------------------------------------------------------===
// SCCP (Sparse Conditional Constant Propagation) Pass
// ===----------------------------------------------------------------------===

SCCPPass::LatticeCell SCCPPass::meet(const LatticeCell& A, const LatticeCell& B) {
  // Lattice meet operation: Undefined < Constant < Overdefined
  if (A.State == Undefined) return B;
  if (B.State == Undefined) return A;
  if (A.State == Overdefined || B.State == Overdefined) {
    return {Overdefined, 0};
  }
  // Both are constants
  if (A.ConstVal == B.ConstVal) {
    return A;
  }
  // Different constants -> overdefined
  return {Overdefined, 0};
}

void SCCPPass::markConstant(IRValue* V, int64_t Val) {
  LatticeCell& Cell = ValueState[V];
  if (Cell.State == Overdefined) return;  // Can't go back from overdefined

  LatticeCell NewCell = {Constant, Val};
  LatticeCell MeetResult = meet(Cell, NewCell);

  if (MeetResult.State != Cell.State ||
      (MeetResult.State == Constant && MeetResult.ConstVal != Cell.ConstVal)) {
    Cell = MeetResult;
    // Add users to worklist (instructions that use this value)
    // For now, we'll process all instructions in executable blocks
  }
}

void SCCPPass::markOverdefined(IRValue* V) {
  LatticeCell& Cell = ValueState[V];
  if (Cell.State == Overdefined) return;  // Already overdefined

  Cell.State = Overdefined;
  Cell.ConstVal = 0;
}

void SCCPPass::markEdgeExecutable(IRBasicBlock* From, IRBasicBlock* To) {
  auto Edge = std::make_pair(From, To);
  if (ExecutableEdges.count(Edge)) {
    return;  // Already marked
  }

  ExecutableEdges.insert(Edge);
  CFGWorkList.push_back(Edge);
}

void SCCPPass::markBlockExecutable(IRBasicBlock* BB) {
  if (ExecutableBlocks.count(BB)) {
    return;  // Already executable
  }

  ExecutableBlocks.insert(BB);

  // Add all instructions in this block to the worklist
  for (const auto& Inst : BB->getInstructions()) {
    SSAWorkList.push_back(Inst.get());
  }
}

bool SCCPPass::tryEvaluateBinary(IRInstruction::Opcode Op, int64_t LHS, int64_t RHS, int64_t& Result) {
  switch (Op) {
    case IRInstruction::Add: Result = LHS + RHS; return true;
    case IRInstruction::Sub: Result = LHS - RHS; return true;
    case IRInstruction::Mul: Result = LHS * RHS; return true;
    case IRInstruction::Div:
      if (RHS == 0) return false;
      Result = LHS / RHS;
      return true;
    case IRInstruction::Mod:
      if (RHS == 0) return false;
      Result = LHS % RHS;
      return true;
    case IRInstruction::And: Result = LHS & RHS; return true;
    case IRInstruction::Or:  Result = LHS | RHS; return true;
    case IRInstruction::Xor: Result = LHS ^ RHS; return true;
    case IRInstruction::Shl: Result = LHS << RHS; return true;
    case IRInstruction::Shr: Result = LHS >> RHS; return true;
    case IRInstruction::Lt:  Result = LHS < RHS ? 1 : 0; return true;
    case IRInstruction::Le:  Result = LHS <= RHS ? 1 : 0; return true;
    case IRInstruction::Gt:  Result = LHS > RHS ? 1 : 0; return true;
    case IRInstruction::Ge:  Result = LHS >= RHS ? 1 : 0; return true;
    case IRInstruction::Eq:  Result = LHS == RHS ? 1 : 0; return true;
    case IRInstruction::Ne:  Result = LHS != RHS ? 1 : 0; return true;
    default: return false;
  }
}

bool SCCPPass::tryEvaluateUnary(IRInstruction::Opcode Op, int64_t Operand, int64_t& Result) {
  switch (Op) {
    case IRInstruction::Not:
      Result = !Operand ? 1 : 0;
      return true;
    default:
      return false;
  }
}

void SCCPPass::visitBinaryInst(IRBinaryInst* BinOp) {
  IRValue* LHS = BinOp->getLHS();
  IRValue* RHS = BinOp->getRHS();

  // Get lattice values for operands
  LatticeCell LHSCell = LHS->isConstant() ?
    LatticeCell{Constant, LHS->getConstant()} : ValueState[LHS];
  LatticeCell RHSCell = RHS->isConstant() ?
    LatticeCell{Constant, RHS->getConstant()} : ValueState[RHS];

  // If either operand is undefined, result is undefined (do nothing)
  if (LHSCell.State == Undefined || RHSCell.State == Undefined) {
    return;
  }

  // If either operand is overdefined, result is overdefined
  if (LHSCell.State == Overdefined || RHSCell.State == Overdefined) {
    markOverdefined(BinOp->getResult());
    return;
  }

  // Both operands are constants - try to fold
  int64_t Result;
  if (tryEvaluateBinary(BinOp->getOpcode(), LHSCell.ConstVal, RHSCell.ConstVal, Result)) {
    markConstant(BinOp->getResult(), Result);
  } else {
    markOverdefined(BinOp->getResult());
  }
}

void SCCPPass::visitUnaryInst(IRUnaryInst* UnOp) {
  IRValue* Operand = UnOp->getOperand();

  LatticeCell OpCell = Operand->isConstant() ?
    LatticeCell{Constant, Operand->getConstant()} : ValueState[Operand];

  if (OpCell.State == Undefined) {
    return;
  }

  if (OpCell.State == Overdefined) {
    markOverdefined(UnOp->getResult());
    return;
  }

  // Operand is constant - try to fold
  int64_t Result;
  if (tryEvaluateUnary(UnOp->getOpcode(), OpCell.ConstVal, Result)) {
    markConstant(UnOp->getResult(), Result);
  } else {
    markOverdefined(UnOp->getResult());
  }
}

void SCCPPass::visitPhi(IRPhiInst* Phi) {
  // Phi node: meet all incoming values from executable edges
  LatticeCell Result = {Undefined, 0};

  for (const auto& Entry : Phi->getIncomings()) {
    IRBasicBlock* IncomingBlock = Entry.Block;
    IRValue* IncomingValue = Entry.Value;

    // Only consider edges that are executable
    bool EdgeExecutable = false;
    for (IRBasicBlock* Pred : Phi->getParent()->getPredecessors()) {
      if (Pred == IncomingBlock &&
          ExecutableEdges.count(std::make_pair(Pred, Phi->getParent()))) {
        EdgeExecutable = true;
        break;
      }
    }

    if (!EdgeExecutable) continue;

    // Get lattice value for incoming value
    LatticeCell IncomingCell = IncomingValue->isConstant() ?
      LatticeCell{Constant, IncomingValue->getConstant()} : ValueState[IncomingValue];

    Result = meet(Result, IncomingCell);
  }

  // Update phi result
  LatticeCell& PhiCell = ValueState[Phi->getResult()];
  if (Result.State != PhiCell.State ||
      (Result.State == Constant && Result.ConstVal != PhiCell.ConstVal)) {
    PhiCell = Result;
  }
}

void SCCPPass::visitCondBr(IRCondBrInst* Br) {
  IRValue* Cond = Br->getCondition();

  LatticeCell CondCell = Cond->isConstant() ?
    LatticeCell{Constant, Cond->getConstant()} : ValueState[Cond];

  if (CondCell.State == Undefined) {
    // Don't mark any edges yet
    return;
  }

  IRBasicBlock* Parent = Br->getParent();

  if (CondCell.State == Overdefined) {
    // Both edges are executable
    // Find the target blocks
    for (IRBasicBlock* Succ : Parent->getSuccessors()) {
      markEdgeExecutable(Parent, Succ);
      markBlockExecutable(Succ);
    }
    return;
  }

  // Condition is constant - only one edge is executable
  // For now, mark both edges (full implementation would only mark the taken edge)
  // This requires mapping labels to blocks
  for (IRBasicBlock* Succ : Parent->getSuccessors()) {
    markEdgeExecutable(Parent, Succ);
    markBlockExecutable(Succ);
  }
}

void SCCPPass::visitInst(IRInstruction* I) {
  if (auto* BinOp = dynamic_cast<IRBinaryInst*>(I)) {
    visitBinaryInst(BinOp);
  } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(I)) {
    visitUnaryInst(UnOp);
  } else if (auto* Phi = dynamic_cast<IRPhiInst*>(I)) {
    visitPhi(Phi);
  } else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(I)) {
    visitCondBr(CondBr);
  } else if (auto* Br = dynamic_cast<IRBrInst*>(I)) {
    // Unconditional branch - mark successor executable
    IRBasicBlock* Parent = Br->getParent();
    for (IRBasicBlock* Succ : Parent->getSuccessors()) {
      markEdgeExecutable(Parent, Succ);
      markBlockExecutable(Succ);
    }
  } else if (auto* Ret = dynamic_cast<IRRetInst*>(I)) {
    // Return - mark return value overdefined if present
    if (Ret->hasRetValue() && !Ret->getRetValue()->isConstant()) {
      markOverdefined(Ret->getRetValue());
    }
  }
  // Other instructions don't affect propagation
}

void SCCPPass::rewriteFunction(IRFunction* F) {
  // Replace all uses of constant values with actual constants
  // This is a simplified version - full implementation would replace operands

  for (auto& BB : F->getBlocks()) {
    for (auto& Inst : BB->getInstructions()) {
      // Replace binary instruction operands if they're constant
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        IRValue* LHS = BinOp->getLHS();
        IRValue* RHS = BinOp->getRHS();

        // If result is constant, we could replace the entire instruction
        // For now, just record the information
      }
    }
  }
}

bool SCCPPass::run(IRFunction* F, AnalysisManager& AM) {
  (void)AM;  // Unused for now

  // Initialize
  ValueState.clear();
  ExecutableEdges.clear();
  ExecutableBlocks.clear();
  SSAWorkList.clear();
  CFGWorkList.clear();

  // Mark entry block as executable
  if (F->getBlocks().empty()) return false;

  IRBasicBlock* Entry = F->getBlocks()[0].get();
  markBlockExecutable(Entry);

  // Worklist algorithm
  while (!SSAWorkList.empty() || !CFGWorkList.empty()) {
    // Process CFG edges first
    while (!CFGWorkList.empty()) {
      auto Edge = CFGWorkList.back();
      CFGWorkList.pop_back();

      IRBasicBlock* To = Edge.second;

      // Re-evaluate all phi nodes in the destination block
      for (const auto& Inst : To->getInstructions()) {
        if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
          visitPhi(Phi);
        }
      }
    }

    // Process SSA instructions
    while (!SSAWorkList.empty()) {
      IRInstruction* I = SSAWorkList.back();
      SSAWorkList.pop_back();

      // Only process instructions in executable blocks
      if (!ExecutableBlocks.count(I->getParent())) {
        continue;
      }

      visitInst(I);
    }
  }

  // Rewrite the function based on discovered constants
  bool Changed = false;

  // For now, just report what we found
  // Full implementation would replace values and remove dead code

  return Changed;
}

// ===----------------------------------------------------------------------===
// GVN (Global Value Numbering) Pass
// ===----------------------------------------------------------------------===

bool GVNPass::Expression::operator<(const Expression& Other) const {
  if (Op != Other.Op) return Op < Other.Op;
  if (Operands.size() != Other.Operands.size()) {
    return Operands.size() < Other.Operands.size();
  }
  for (size_t i = 0; i < Operands.size(); ++i) {
    if (Operands[i] != Other.Operands[i]) {
      return Operands[i] < Other.Operands[i];
    }
  }
  return false;
}

GVNPass::Expression GVNPass::createExpression(IRInstruction* I) {
  Expression Expr;
  Expr.Op = I->getOpcode();

  if (auto* BinOp = dynamic_cast<IRBinaryInst*>(I)) {
    Expr.Operands.push_back(BinOp->getLHS());
    Expr.Operands.push_back(BinOp->getRHS());
  } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(I)) {
    Expr.Operands.push_back(UnOp->getOperand());
  } else if (auto* Load = dynamic_cast<IRLoadInst*>(I)) {
    Expr.Operands.push_back(Load->getPtr());
  }

  return Expr;
}

IRValue* GVNPass::findExistingComputation(const Expression& Expr) {
  auto It = ExpressionMap.find(Expr);
  if (It != ExpressionMap.end()) {
    return It->second;
  }
  return nullptr;
}

void GVNPass::replaceAllUsesWith(IRFunction* F, IRValue* Old, IRValue* New) {
  // Replace all uses of Old with New in the function
  for (auto& BB : F->getBlocks()) {
    for (auto& Inst : BB->getInstructions()) {
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        if (BinOp->getLHS() == Old) BinOp->setLHS(New);
        if (BinOp->getRHS() == Old) BinOp->setRHS(New);
      } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
        if (UnOp->getOperand() == Old) UnOp->setOperand(New);
      } else if (auto* Store = dynamic_cast<IRStoreInst*>(Inst.get())) {
        if (Store->getValue() == Old) Store->setValue(New);
        if (Store->getPtr() == Old) Store->setPtr(New);
      } else if (auto* Ret = dynamic_cast<IRRetInst*>(Inst.get())) {
        if (Ret->hasRetValue() && Ret->getRetValue() == Old) {
          Ret->setRetValue(New);
        }
      } else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(Inst.get())) {
        if (CondBr->getCondition() == Old) CondBr->setCondition(New);
      } else if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
        Phi->replaceIncomingValue(Old, New);
      }
    }
  }
}

bool GVNPass::run(IRFunction* F, AnalysisManager& AM) {
  (void)AM;  // Unused

  bool Changed = false;
  ExpressionMap.clear();
  Replacements.clear();

  // Simple GVN: walk through all instructions and look for redundant computations
  // This is a basic local GVN (within basic blocks)

  for (auto& BB : F->getBlocks()) {
    // Clear expression map at start of each block (local GVN)
    ExpressionMap.clear();

    for (auto& Inst : BB->getInstructions()) {
      // Only handle pure instructions (no side effects)
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        Expression Expr = createExpression(Inst.get());
        IRValue* Existing = findExistingComputation(Expr);

        if (Existing) {
          // Found redundant computation!
          Replacements[BinOp->getResult()] = Existing;
          Changed = true;
        } else {
          // Record this computation
          ExpressionMap[Expr] = BinOp->getResult();
        }
      } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
        Expression Expr = createExpression(Inst.get());
        IRValue* Existing = findExistingComputation(Expr);

        if (Existing) {
          Replacements[UnOp->getResult()] = Existing;
          Changed = true;
        } else {
          ExpressionMap[Expr] = UnOp->getResult();
        }
      }
      // Loads are tricky - only safe to eliminate if no stores in between
      // Skip for now
    }
  }

  // Apply replacements
  for (const auto& Entry : Replacements) {
    replaceAllUsesWith(F, Entry.first, Entry.second);
  }

  return Changed;
}

// ===----------------------------------------------------------------------===
// LICM (Loop Invariant Code Motion) Pass
// ===----------------------------------------------------------------------===

bool LICMPass::isLoopInvariant(IRInstruction* I, Loop* L,
                                const std::set<IRValue*>& LoopInvariants) {
  // Check if all operands are either:
  // 1. Constants
  // 2. Defined outside the loop
  // 3. Already marked as loop invariant

  if (auto* BinOp = dynamic_cast<IRBinaryInst*>(I)) {
    IRValue* LHS = BinOp->getLHS();
    IRValue* RHS = BinOp->getRHS();

    bool LHSInvariant = LHS->isConstant() || LoopInvariants.count(LHS) ||
                        !L->contains(LHS->getType() ? nullptr : nullptr);  // Simplified check
    bool RHSInvariant = RHS->isConstant() || LoopInvariants.count(RHS);

    return LHSInvariant && RHSInvariant;
  } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(I)) {
    IRValue* Op = UnOp->getOperand();
    return Op->isConstant() || LoopInvariants.count(Op);
  }

  // Other instructions: assume not invariant for safety
  return false;
}

bool LICMPass::isSafeToHoist(IRInstruction* I, Loop* L) {
  // Check if it's safe to move this instruction:
  // 1. No side effects (no stores, calls, etc.)
  // 2. Dominates all loop exits (for correctness)

  // For now, only allow pure arithmetic and logical operations
  if (dynamic_cast<IRBinaryInst*>(I) || dynamic_cast<IRUnaryInst*>(I)) {
    return true;
  }

  // Don't hoist loads (alias analysis needed)
  // Don't hoist stores (side effects)
  // Don't hoist calls (side effects)
  return false;
}

void LICMPass::hoistInstruction(IRInstruction* I, IRBasicBlock* Preheader) {
  // Get the current block
  IRBasicBlock* CurrentBB = I->getParent();
  if (!CurrentBB) return;

  // Remove from current block
  auto Inst = CurrentBB->removeInstruction(I);
  if (!Inst) return;

  // Insert into preheader (before terminator)
  Preheader->insertBeforeTerminator(std::move(Inst));
}

bool LICMPass::run(IRFunction* F, AnalysisManager& AM) {
  // Get loop information
  LoopInfo& LI = AM.get<LoopInfo>();

  bool Changed = false;

  // Process each loop
  for (const auto& L : LI.getTopLevelLoops()) {
    // Need a preheader to hoist to
    if (!L->getPreheader()) {
      continue;
    }

    IRBasicBlock* Preheader = L->getPreheader();

    // Track loop-invariant values and instructions to hoist
    std::set<IRValue*> LoopInvariants;
    std::vector<IRInstruction*> ToHoist;

    // Iterate until no more invariants found
    bool LocalChanged = true;
    while (LocalChanged) {
      LocalChanged = false;

      // Check each instruction in the loop
      for (IRBasicBlock* BB : L->getBlocks()) {
        for (const auto& Inst : BB->getInstructions()) {
          // Skip if already determined to be invariant
          if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
            if (LoopInvariants.count(BinOp->getResult())) {
              continue;
            }

            // Check if invariant
            if (isLoopInvariant(Inst.get(), L.get(), LoopInvariants)) {
              if (isSafeToHoist(Inst.get(), L.get())) {
                LoopInvariants.insert(BinOp->getResult());
                ToHoist.push_back(Inst.get());
                LocalChanged = true;
                Changed = true;
              }
            }
          } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
            if (LoopInvariants.count(UnOp->getResult())) {
              continue;
            }

            if (isLoopInvariant(Inst.get(), L.get(), LoopInvariants)) {
              if (isSafeToHoist(Inst.get(), L.get())) {
                LoopInvariants.insert(UnOp->getResult());
                ToHoist.push_back(Inst.get());
                LocalChanged = true;
                Changed = true;
              }
            }
          }
        }
      }
    }

    // Actually hoist the instructions
    for (IRInstruction* Inst : ToHoist) {
      hoistInstruction(Inst, Preheader);
    }
  }

  return Changed;
}

// ===----------------------------------------------------------------------===
// Inlining Pass
// ===----------------------------------------------------------------------===

size_t InliningPass::calculateInlineCost(IRFunction* Callee) {
  // Simple cost model: count instructions
  size_t Cost = 0;
  for (const auto& BB : Callee->getBlocks()) {
    Cost += BB->getInstructions().size();
  }
  return Cost;
}

bool InliningPass::isInlinable(IRFunction* Callee) {
  // Check if function is suitable for inlining:
  // 1. Not too large
  // 2. Doesn't recursively call itself
  // 3. Has a simple structure

  if (calculateInlineCost(Callee) > InlineBudget) {
    return false;
  }

  // Check for recursive calls
  for (const auto& BB : Callee->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      if (auto* Call = dynamic_cast<IRCallInst*>(Inst.get())) {
        if (Call->getFuncName() == Callee->getName()) {
          return false;  // Recursive
        }
      }
    }
  }

  return true;
}

bool InliningPass::inlineCallSite(IRCallInst* Call, IRFunction* Callee, IRFunction* Caller) {
  // Simplified inlining: just mark the decision
  // Full implementation would:
  // 1. Clone callee's IR
  // 2. Remap values
  // 3. Replace call with inlined code
  // 4. Update phi nodes

  // For now, return false (no actual inlining)
  return false;
}

bool InliningPass::run(IRFunction* F, AnalysisManager& AM) {
  // Inlining requires module-level analysis (cross-function)
  // For now, just return false (not implemented at function level)
  // Would need a module-level pass manager
  return false;
}

// ===----------------------------------------------------------------------===
// Loop Unrolling Pass
// ===----------------------------------------------------------------------===

bool LoopUnrollPass::getTripCount(Loop* L, int64_t& Count) {
  // Try to determine if the loop has a constant trip count
  // This requires analyzing the loop condition and increment
  // For simplicity, return false (unknown trip count)
  return false;
}

bool LoopUnrollPass::canUnroll(Loop* L) {
  // Check if loop is suitable for unrolling:
  // 1. Small body
  // 2. Constant trip count (if full unrolling)
  // 3. No complex control flow

  size_t BodySize = 0;
  for (IRBasicBlock* BB : L->getBlocks()) {
    BodySize += BB->getInstructions().size();
  }

  // Don't unroll large loops
  if (BodySize > 20) {
    return false;
  }

  return true;
}

bool LoopUnrollPass::unrollLoop(Loop* L, unsigned Factor) {
  // Simplified unrolling: just check if we should
  // Full implementation would:
  // 1. Clone loop body Factor times
  // 2. Update phi nodes
  // 3. Adjust loop condition
  // 4. Update branch targets

  // For now, return false (not implemented)
  return false;
}

bool LoopUnrollPass::run(IRFunction* F, AnalysisManager& AM) {
  // Get loop information
  LoopInfo& LI = AM.get<LoopInfo>();

  bool Changed = false;

  // Try to unroll each loop
  for (const auto& L : LI.getTopLevelLoops()) {
    if (canUnroll(L.get())) {
      // Check for constant trip count
      int64_t TripCount;
      if (getTripCount(L.get(), TripCount)) {
        // Full unroll if small trip count
        if (TripCount > 0 && TripCount <= 8) {
          if (unrollLoop(L.get(), static_cast<unsigned>(TripCount))) {
            Changed = true;
          }
        }
      } else {
        // Partial unroll
        if (unrollLoop(L.get(), UnrollFactor)) {
          Changed = true;
        }
      }
    }
  }

  return Changed;
}

} // namespace yac
