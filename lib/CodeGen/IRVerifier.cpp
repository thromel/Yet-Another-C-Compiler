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
      // Check operands are defined
      // This is simplified - a real verifier would extract all operands
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
      // TODO: Add checks for other instruction types
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
