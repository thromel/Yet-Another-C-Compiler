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

} // namespace yac

#endif // YAC_CODEGEN_TRANSFORMS_H
