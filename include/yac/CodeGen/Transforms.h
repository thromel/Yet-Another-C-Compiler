#ifndef YAC_CODEGEN_TRANSFORMS_H
#define YAC_CODEGEN_TRANSFORMS_H

#include "yac/CodeGen/IR.h"
#include "yac/CodeGen/Pass.h"
#include <map>
#include <set>
#include <vector>

namespace yac {

/// Mem2Reg - Promote memory to register (alloca → SSA)
/// Converts alloca/load/store to SSA form with phi nodes
class Mem2RegPass : public Pass {
public:
  std::string getName() const override { return "Mem2Reg"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  // Per-alloca state
  struct AllocaInfo {
    IRAllocaInst* Alloca;
    std::vector<IRStoreInst*> DefiningStores;
    std::vector<IRLoadInst*> Uses;
    std::set<IRBasicBlock*> DefBlocks;
    std::map<IRBasicBlock*, IRPhiInst*> PhiNodes;  // Maps block to phi for this alloca
    std::map<IRValue*, IRValue*> Replacements;  // Maps old value to new SSA value
    bool IsPromotable = true;
  };

  IRFunction* CurrentFunc = nullptr;
  DominatorTree* DT = nullptr;

  // Helpers
  void identifyPromotableAllocas(
      std::vector<AllocaInfo>& Allocas);

  void computeDefBlocks(AllocaInfo& Info);

  void insertPhiNodes(AllocaInfo& Info);

  void renameVariables(AllocaInfo& Info);

  void renameInBlock(
      AllocaInfo& Info,
      IRBasicBlock* BB,
      std::map<IRBasicBlock*, IRValue*>& CurrentDef,
      std::set<IRBasicBlock*>& Visited);

  std::set<IRBasicBlock*> computeDominanceFrontier(
      const std::set<IRBasicBlock*>& Blocks);

  void replaceLoadsAndRemoveStores(AllocaInfo& Info);

  void replaceValueInInstruction(IRInstruction* Inst,
                                   IRValue* Old,
                                   IRValue* New);

  IRValue* createSSAValue(IRValue* OrigValue);
};

/// DeadCodeElimination - Remove unused instructions
class DCEPass : public Pass {
public:
  std::string getName() const override { return "DCE"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  bool isInstructionDead(IRInstruction* I);
  void collectLiveInstructions(
      IRFunction* F,
      std::set<IRInstruction*>& Live);
};

/// ConstantPropagation - Propagate and fold constants
class ConstantPropagationPass : public Pass {
public:
  std::string getName() const override { return "ConstProp"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  // Maps SSA values to their constant value (if known)
  std::map<IRValue*, int64_t> ConstantValues;

  bool isConstant(IRValue* V);
  int64_t getConstant(IRValue* V);
  IRValue* tryFoldBinary(IRBinaryInst* BinOp);
  IRValue* tryFoldCompare(IRInstruction* Cmp);
};

/// SimplifyCFG - Simplify control flow graph
class SimplifyCFGPass : public Pass {
public:
  std::string getName() const override { return "SimplifyCFG"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return false; }
  bool preservesInstructions() const override { return false; }

private:
  bool mergeBlocks(IRBasicBlock* Pred, IRBasicBlock* Succ);
  bool removeUnreachableBlocks(IRFunction* F);
};

/// CopyPropagation - Eliminate redundant copies/moves
/// Replaces uses of copied values with original values
class CopyPropagationPass : public Pass {
public:
  std::string getName() const override { return "CopyProp"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  // Maps copied values to their source values
  std::map<IRValue*, IRValue*> CopyMap;

  void buildCopyMap(IRFunction* F);
  IRValue* getOriginalValue(IRValue* V);
  void replaceValueInInstruction(IRInstruction* Inst, IRValue* Old, IRValue* New);
};

} // namespace yac

#endif // YAC_CODEGEN_TRANSFORMS_H
