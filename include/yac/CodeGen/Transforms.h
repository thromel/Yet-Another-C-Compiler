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

/// SCCP - Sparse Conditional Constant Propagation
/// More powerful than simple constant propagation:
/// - Propagates through phi nodes
/// - Marks unreachable code via constant branch conditions
/// - Uses worklist algorithm for efficiency
class SCCPPass : public Pass {
public:
  std::string getName() const override { return "SCCP"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  enum LatticeValue {
    Undefined,    // Not yet computed
    Constant,     // Known constant value
    Overdefined   // Not constant (multiple values or unknown)
  };

  struct LatticeCell {
    LatticeValue State = Undefined;
    int64_t ConstVal = 0;
  };

  // Lattice values for each SSA value
  std::map<IRValue*, LatticeCell> ValueState;

  // Executable edges and blocks
  std::set<std::pair<IRBasicBlock*, IRBasicBlock*>> ExecutableEdges;
  std::set<IRBasicBlock*> ExecutableBlocks;

  // Worklist for propagation
  std::vector<IRInstruction*> SSAWorkList;
  std::vector<std::pair<IRBasicBlock*, IRBasicBlock*>> CFGWorkList;

  // Lattice operations
  LatticeCell meet(const LatticeCell& A, const LatticeCell& B);
  void markConstant(IRValue* V, int64_t Val);
  void markOverdefined(IRValue* V);

  // Worklist management
  void markEdgeExecutable(IRBasicBlock* From, IRBasicBlock* To);
  void markBlockExecutable(IRBasicBlock* BB);
  void visitInst(IRInstruction* I);
  void visitPhi(IRPhiInst* Phi);
  void visitBinaryInst(IRBinaryInst* BinOp);
  void visitUnaryInst(IRUnaryInst* UnOp);
  void visitCondBr(IRCondBrInst* Br);

  // Evaluation
  bool tryEvaluateBinary(IRInstruction::Opcode Op, int64_t LHS, int64_t RHS, int64_t& Result);
  bool tryEvaluateUnary(IRInstruction::Opcode Op, int64_t Operand, int64_t& Result);

  // Rewriting
  void rewriteFunction(IRFunction* F);
};

/// GVN - Global Value Numbering (lite version)
/// Performs common subexpression elimination (CSE)
/// Identifies and eliminates redundant computations
class GVNPass : public Pass {
public:
  std::string getName() const override { return "GVN"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  // Expression representation for hashing
  struct Expression {
    IRInstruction::Opcode Op;
    std::vector<IRValue*> Operands;

    bool operator<(const Expression& Other) const;
  };

  // Value numbering maps
  std::map<Expression, IRValue*> ExpressionMap;
  std::map<IRValue*, IRValue*> Replacements;

  // Build expression from instruction
  Expression createExpression(IRInstruction* I);

  // Try to find existing computation
  IRValue* findExistingComputation(const Expression& Expr);

  // Replace all uses of a value
  void replaceAllUsesWith(IRFunction* F, IRValue* Old, IRValue* New);
};

/// LICM - Loop Invariant Code Motion
/// Moves loop-invariant computations out of loops
class LICMPass : public Pass {
public:
  std::string getName() const override { return "LICM"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return true; }
  bool preservesInstructions() const override { return false; }

private:
  // Check if an instruction is loop invariant
  bool isLoopInvariant(IRInstruction* I, Loop* L, const std::set<IRValue*>& LoopInvariants);

  // Check if it's safe to hoist an instruction
  bool isSafeToHoist(IRInstruction* I, Loop* L);

  // Hoist instruction to preheader
  void hoistInstruction(IRInstruction* I, IRBasicBlock* Preheader);
};

/// Inlining - Function inlining with cost budget
/// Inlines small functions to eliminate call overhead
class InliningPass : public Pass {
public:
  InliningPass(size_t Budget = 50) : InlineBudget(Budget) {}

  std::string getName() const override { return "Inline"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return false; }
  bool preservesInstructions() const override { return false; }

private:
  size_t InlineBudget;  // Max instruction count to inline

  // Calculate cost of inlining a function
  size_t calculateInlineCost(IRFunction* Callee);

  // Check if function is inlinable
  bool isInlinable(IRFunction* Callee);

  // Inline a call site
  bool inlineCallSite(IRCallInst* Call, IRFunction* Callee, IRFunction* Caller);
};

/// LoopUnrolling - Unroll loops with small constant trip counts
/// Reduces loop overhead and enables more optimizations
class LoopUnrollPass : public Pass {
public:
  LoopUnrollPass(unsigned Factor = 4) : UnrollFactor(Factor) {}

  std::string getName() const override { return "LoopUnroll"; }
  bool run(IRFunction* F, AnalysisManager& AM) override;

  bool preservesCFG() const override { return false; }
  bool preservesInstructions() const override { return false; }

private:
  unsigned UnrollFactor;  // How many times to unroll

  // Check if loop can be unrolled
  bool canUnroll(Loop* L);

  // Get trip count if it's a constant
  bool getTripCount(Loop* L, int64_t& Count);

  // Unroll the loop
  bool unrollLoop(Loop* L, unsigned Factor);
};

} // namespace yac

#endif // YAC_CODEGEN_TRANSFORMS_H
