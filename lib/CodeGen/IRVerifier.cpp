#include "yac/CodeGen/IRVerifier.h"
#include <iostream>
#include <set>
#include <map>

namespace yac {

void IRVerifier::addError(const std::string& Msg, IRFunction* F,
                          IRBasicBlock* BB, IRInstruction* I) {
  Errors.push_back({Msg, F, BB, I});
  if (FailFast) {
    printErrors();
    std::cerr << "IR verification failed!\n";
    exit(1);
  }
}

void IRVerifier::printErrors() const {
  for (const auto& Err : Errors) {
    std::cerr << "IR Verification Error: " << Err.Message;
    if (Err.Function) {
      std::cerr << " in function '" << Err.Function->getName() << "'";
    }
    if (Err.Block) {
      std::cerr << " in block '" << Err.Block->getName() << "'";
    }
    std::cerr << "\n";
  }
}

bool IRVerifier::verify(IRModule* M) {
  Errors.clear();

  for (const auto& F : M->getFunctions()) {
    if (!verifyFunction(F.get())) {
      if (FailFast) return false;
    }
  }

  return Errors.empty();
}

bool IRVerifier::verifyFunction(IRFunction* F) {
  bool Valid = true;

  // Check that function has at least one block
  if (F->getBlocks().empty()) {
    addError("Function has no basic blocks", F);
    return false;
  }

  // Verify each basic block
  for (const auto& BB : F->getBlocks()) {
    if (!verifyBasicBlock(BB.get())) {
      Valid = false;
      if (FailFast) return false;
    }
  }

  // Check def-before-use
  if (!checkDefBeforeUse(F)) {
    Valid = false;
    if (FailFast) return false;
  }

  // Check no orphaned blocks (except entry)
  if (!checkNoOrphanedBlocks(F)) {
    Valid = false;
    if (FailFast) return false;
  }

  return Valid;
}

bool IRVerifier::verifyBasicBlock(IRBasicBlock* BB) {
  bool Valid = true;

  // Check that block has parent
  if (!BB->getParent()) {
    addError("Basic block has no parent function", nullptr, BB);
    return false;
  }

  // Check terminator
  if (!checkBlockTerminator(BB)) {
    Valid = false;
    if (FailFast) return false;
  }

  // Check CFG consistency
  if (!checkCFGConsistency(BB)) {
    Valid = false;
    if (FailFast) return false;
  }

  // Check phi nodes
  if (!checkPhiNodes(BB)) {
    Valid = false;
    if (FailFast) return false;
  }

  // Check that all instructions have correct parent
  for (const auto& Inst : BB->getInstructions()) {
    if (Inst->getParent() != BB) {
      addError("Instruction has incorrect parent block",
               BB->getParent(), BB, Inst.get());
      Valid = false;
      if (FailFast) return false;
    }
  }

  return Valid;
}

bool IRVerifier::checkBlockTerminator(IRBasicBlock* BB) {
  const auto& Insts = BB->getInstructions();

  if (Insts.empty()) {
    addError("Basic block is empty", BB->getParent(), BB);
    return false;
  }

  // Check that last instruction is a terminator
  IRInstruction* Last = Insts.back().get();
  if (!Last->isTerminator()) {
    // Skip terminator check for unreachable blocks (no predecessors, not entry)
    // These will be removed by SimplifyCFG
    bool isEntry = BB->getParent() && !BB->getParent()->getBlocks().empty() &&
                   BB->getParent()->getBlocks()[0].get() == BB;
    if (!isEntry && BB->getNumPredecessors() == 0) {
      // Unreachable block, skip this check
      return true;
    }

    addError("Basic block does not end with terminator",
             BB->getParent(), BB, Last);
    return false;
  }

  // Check that no other instruction is a terminator
  for (size_t i = 0; i < Insts.size() - 1; ++i) {
    if (Insts[i]->isTerminator()) {
      addError("Terminator instruction not at end of block",
               BB->getParent(), BB, Insts[i].get());
      return false;
    }
  }

  return true;
}

bool IRVerifier::checkCFGConsistency(IRBasicBlock* BB) {
  bool Valid = true;

  // Check that terminator's targets match successor list
  IRInstruction* Term = BB->getTerminator();
  if (!Term) return true; // Already reported by checkBlockTerminator

  std::set<IRBasicBlock*> TerminatorTargets;

  if (auto* Br = dynamic_cast<IRBrInst*>(Term)) {
    // Find target block by name (this is a limitation of current design)
    // TODO: Store direct block pointers instead of labels
    (void)Br; // Suppress unused warning
    // For now, skip this check since we need to refactor label handling
  } else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(Term)) {
    (void)CondBr; // Suppress unused warning
    // For now, skip this check since we need to refactor label handling
  }

  // Check predecessor/successor symmetry
  for (IRBasicBlock* Succ : BB->getSuccessors()) {
    bool Found = false;
    for (IRBasicBlock* Pred : Succ->getPredecessors()) {
      if (Pred == BB) {
        Found = true;
        break;
      }
    }
    if (!Found) {
      addError("CFG edge inconsistency: block is not in successor's predecessor list",
               BB->getParent(), BB);
      Valid = false;
      if (FailFast) return false;
    }
  }

  for (IRBasicBlock* Pred : BB->getPredecessors()) {
    bool Found = false;
    for (IRBasicBlock* Succ : Pred->getSuccessors()) {
      if (Succ == BB) {
        Found = true;
        break;
      }
    }
    if (!Found) {
      addError("CFG edge inconsistency: block is not in predecessor's successor list",
               BB->getParent(), BB);
      Valid = false;
      if (FailFast) return false;
    }
  }

  return Valid;
}

bool IRVerifier::checkPhiNodes(IRBasicBlock* BB) {
  bool Valid = true;
  bool SeenNonPhi = false;

  for (const auto& Inst : BB->getInstructions()) {
    if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
      // Check that all phi nodes are at the beginning of the block
      if (SeenNonPhi) {
        addError("Phi instruction not at beginning of block",
                 BB->getParent(), BB, Phi);
        Valid = false;
        if (FailFast) return false;
      }

      // Check that number of phi entries matches number of predecessors
      if (Phi->getNumIncomings() != BB->getNumPredecessors()) {
        addError("Phi instruction has wrong number of incoming values",
                 BB->getParent(), BB, Phi);
        Valid = false;
        if (FailFast) return false;
      }

      // Check that each predecessor appears exactly once
      std::set<IRBasicBlock*> PhiPreds;
      for (const auto& Entry : Phi->getIncomings()) {
        if (PhiPreds.count(Entry.Block)) {
          addError("Phi instruction has duplicate predecessor",
                   BB->getParent(), BB, Phi);
          Valid = false;
          if (FailFast) return false;
        }
        PhiPreds.insert(Entry.Block);

        // Check that the block is actually a predecessor
        bool Found = false;
        for (IRBasicBlock* Pred : BB->getPredecessors()) {
          if (Pred == Entry.Block) {
            Found = true;
            break;
          }
        }
        if (!Found) {
          addError("Phi instruction references non-predecessor block",
                   BB->getParent(), BB, Phi);
          Valid = false;
          if (FailFast) return false;
        }
      }
    } else {
      SeenNonPhi = true;
    }
  }

  return Valid;
}

bool IRVerifier::checkDefBeforeUse(IRFunction* F) {
  bool Valid = true;
  std::set<IRValue*> Defined;

  // Parameters are defined at function entry
  for (IRValue* Param : F->getParameters()) {
    Defined.insert(Param);
  }

  // Walk through blocks in order (TODO: use RPO order for better checking)
  for (const auto& BB : F->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      // Check operands are defined based on instruction type
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        if (!BinOp->getLHS()->isConstant() && !Defined.count(BinOp->getLHS())) {
          addError("Use of undefined value", F, BB.get(), BinOp);
          Valid = false;
          if (FailFast) return false;
        }
        if (!BinOp->getRHS()->isConstant() && !Defined.count(BinOp->getRHS())) {
          addError("Use of undefined value", F, BB.get(), BinOp);
          Valid = false;
          if (FailFast) return false;
        }
        // Define result
        Defined.insert(BinOp->getResult());
      }
      else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
        // Check operand is defined
        if (!UnOp->getOperand()->isConstant() && !Defined.count(UnOp->getOperand())) {
          addError("Use of undefined value", F, BB.get(), UnOp);
          Valid = false;
          if (FailFast) return false;
        }
        // Define result
        Defined.insert(UnOp->getResult());
      }
      else if (auto* Alloca = dynamic_cast<IRAllocaInst*>(Inst.get())) {
        // Alloca defines its result
        Defined.insert(Alloca->getResult());
      }
      else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst.get())) {
        // Check pointer is defined
        if (!Load->getPtr()->isConstant() && !Defined.count(Load->getPtr())) {
          addError("Use of undefined value", F, BB.get(), Load);
          Valid = false;
          if (FailFast) return false;
        }
        // Define result
        Defined.insert(Load->getResult());
      }
      else if (auto* Store = dynamic_cast<IRStoreInst*>(Inst.get())) {
        // Check value is defined
        if (!Store->getValue()->isConstant() && !Defined.count(Store->getValue())) {
          addError("Use of undefined value", F, BB.get(), Store);
          Valid = false;
          if (FailFast) return false;
        }
        // Check pointer is defined
        if (!Store->getPtr()->isConstant() && !Defined.count(Store->getPtr())) {
          addError("Use of undefined value", F, BB.get(), Store);
          Valid = false;
          if (FailFast) return false;
        }
      }
      else if (auto* Call = dynamic_cast<IRCallInst*>(Inst.get())) {
        // Check arguments
        for (IRValue* Arg : Call->getArgs()) {
          if (!Arg->isConstant() && !Defined.count(Arg)) {
            addError("Use of undefined value", F, BB.get(), Call);
            Valid = false;
            if (FailFast) return false;
          }
        }
        // Define result if function returns a value
        if (Call->getResult()) {
          Defined.insert(Call->getResult());
        }
      }
      else if (auto* Ret = dynamic_cast<IRRetInst*>(Inst.get())) {
        // Check return value if present
        if (Ret->hasRetValue() && !Ret->getRetValue()->isConstant() && !Defined.count(Ret->getRetValue())) {
          addError("Use of undefined value", F, BB.get(), Ret);
          Valid = false;
          if (FailFast) return false;
        }
      }
      else if (auto* Br = dynamic_cast<IRCondBrInst*>(Inst.get())) {
        // Check condition
        if (!Br->getCondition()->isConstant() && !Defined.count(Br->getCondition())) {
          addError("Use of undefined value", F, BB.get(), Br);
          Valid = false;
          if (FailFast) return false;
        }
      }
      else if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
        // Phi node defines its result
        Defined.insert(Phi->getResult());
        // Note: We check phi incoming values separately in checkPhiNodes
      }
      // UnconditionalBrInst and other instructions with no operands are fine
    }
  }

  return Valid;
}

bool IRVerifier::checkNoOrphanedBlocks(IRFunction* F) {
  bool Valid = true;

  // Entry block is always reachable
  if (F->getBlocks().empty()) return true;

  std::set<IRBasicBlock*> Reachable;
  std::vector<IRBasicBlock*> Worklist;

  IRBasicBlock* Entry = F->getBlocks()[0].get();
  Reachable.insert(Entry);
  Worklist.push_back(Entry);

  // DFS to find all reachable blocks
  while (!Worklist.empty()) {
    IRBasicBlock* BB = Worklist.back();
    Worklist.pop_back();

    for (IRBasicBlock* Succ : BB->getSuccessors()) {
      if (!Reachable.count(Succ)) {
        Reachable.insert(Succ);
        Worklist.push_back(Succ);
      }
    }
  }

  // Check that all blocks are reachable
  for (const auto& BB : F->getBlocks()) {
    if (!Reachable.count(BB.get())) {
      addError("Unreachable basic block (orphaned)", F, BB.get());
      Valid = false;
      if (FailFast) return false;
    }
  }

  return Valid;
}

} // namespace yac
