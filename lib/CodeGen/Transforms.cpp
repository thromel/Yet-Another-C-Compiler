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

  for (AllocaInfo& Info : Allocas) {
    if (!Info.IsPromotable) continue;

    // Compute which blocks have stores
    computeDefBlocks(Info);

    // Insert phi nodes
    insertPhiNodes(Info);

    // Rename variables (SSA construction)
    renameVariables(Info);

    // Remove dead alloca, loads, stores
    // TODO: Implement dead instruction removal
    // For now, just mark as changed
    Changed = true;
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
          "phi_" + std::to_string(PhiBlocks.size()),
          Info.Alloca->getAllocType());

      auto Phi = std::make_unique<IRPhiInst>(PhiResult);

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

      Phi->setParent(FrontierBlock);
      Insts.insert(Insts.begin() + InsertPos, std::move(Phi));

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
  for (const auto& Inst : BB->getInstructions()) {
    if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
      // TODO: Check if this phi is for our alloca
      // For now, assume it is
      IncomingValue = Phi->getResult();
      break;
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
    // Handle loads: replace with current value
    else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst.get())) {
      if (Load->getPtr() == Info.Alloca->getResult()) {
        if (IncomingValue) {
          // TODO: Replace all uses of Load->getResult() with IncomingValue
          // For now, just note that we should
        }
      }
    }
  }

  // Visit successors and fill in phi nodes
  for (IRBasicBlock* Succ : BB->getSuccessors()) {
    // Find phi nodes in successor
    for (const auto& Inst : Succ->getInstructions()) {
      if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
        // Add incoming value from this block
        if (IncomingValue) {
          Phi->addIncoming(IncomingValue, BB);
        }
      }
    }
  }

  // Recursively visit dominated children
  if (auto* Node = DT->getNode(BB)) {
    for (auto* Child : Node->Children) {
      renameInBlock(Info, Child->Block, CurrentDef, Visited);
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

// ===----------------------------------------------------------------------===
// DCE Pass
// ===----------------------------------------------------------------------===

bool DCEPass::run(IRFunction* F, AnalysisManager& AM) {
  std::set<IRInstruction*> Live;

  // Collect initially live instructions (side effects)
  collectLiveInstructions(F, Live);

  // Mark transitively live instructions
  bool Changed = true;
  while (Changed) {
    Changed = false;
    for (const auto& BB : F->getBlocks()) {
      for (const auto& Inst : BB->getInstructions()) {
        if (Live.count(Inst.get())) {
          // Mark operands as live
          // TODO: Extract operands and mark their defining instructions as live
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

} // namespace yac
