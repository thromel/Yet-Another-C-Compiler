#ifndef YAC_CODEGEN_PASS_H
#define YAC_CODEGEN_PASS_H

#include "yac/CodeGen/IR.h"
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <vector>

namespace yac {

// Forward declarations
class AnalysisManager;
class Pass;

/// Analysis - base class for analyses
class Analysis {
public:
  virtual ~Analysis() = default;
  virtual std::string getName() const = 0;

  // Invalidation flags
  enum InvalidationKind {
    None = 0,
    CFG = 1 << 0,          // CFG structure changed
    Instructions = 1 << 1,  // Instructions added/removed/modified
    Values = 1 << 2,        // Values added/removed
    All = CFG | Instructions | Values
  };

  virtual bool invalidate(IRFunction* F, unsigned Mask) = 0;
};

/// AnalysisManager - manages analyses for a function
class AnalysisManager {
  IRFunction* Func;
  std::map<std::type_index, std::unique_ptr<Analysis>> Analyses;

public:
  AnalysisManager(IRFunction* F) : Func(F) {}

  /// Get or compute an analysis
  template<typename AnalysisT>
  AnalysisT& get() {
    std::type_index Idx = typeid(AnalysisT);

    auto It = Analyses.find(Idx);
    if (It != Analyses.end()) {
      return static_cast<AnalysisT&>(*It->second);
    }

    // Compute the analysis
    auto A = std::make_unique<AnalysisT>();
    A->run(Func);
    AnalysisT* Ptr = A.get();
    Analyses[Idx] = std::move(A);
    return *Ptr;
  }

  /// Invalidate analyses based on mask
  void invalidate(unsigned Mask) {
    for (auto It = Analyses.begin(); It != Analyses.end(); ) {
      if (It->second->invalidate(Func, Mask)) {
        It = Analyses.erase(It);
      } else {
        ++It;
      }
    }
  }

  /// Clear all analyses
  void clear() {
    Analyses.clear();
  }

  IRFunction* getFunction() const { return Func; }
};

/// Pass - base class for transformation passes
class Pass {
public:
  virtual ~Pass() = default;
  virtual std::string getName() const = 0;
  virtual bool run(IRFunction* F, AnalysisManager& AM) = 0;

  // Returns true if the pass modified the IR
  virtual bool preservesCFG() const { return true; }
  virtual bool preservesInstructions() const { return false; }
};

/// PassManager - runs passes on functions and modules
class PassManager {
  std::vector<std::unique_ptr<Pass>> Passes;
  bool VerifyEach = false;

public:
  PassManager(bool VerifyEach = false) : VerifyEach(VerifyEach) {}

  /// Add a pass to the pipeline
  void addPass(std::unique_ptr<Pass> P) {
    Passes.push_back(std::move(P));
  }

  /// Run all passes on a function
  bool run(IRFunction* F);

  /// Run all passes on a module
  bool run(IRModule* M);

  /// Enable/disable verification after each pass
  void setVerifyEach(bool V) { VerifyEach = V; }
};

// ===----------------------------------------------------------------------===
// Common Analyses
// ===----------------------------------------------------------------------===

/// DominatorTree - dominator tree analysis
class DominatorTree : public Analysis {
public:
  struct Node {
    IRBasicBlock* Block;
    Node* IDom = nullptr;  // Immediate dominator
    std::vector<Node*> Children;
  };

private:
  std::map<IRBasicBlock*, Node*> Nodes;
  Node* Root = nullptr;
  std::vector<std::unique_ptr<Node>> AllNodes;

public:
  DominatorTree() = default;

  std::string getName() const override { return "DominatorTree"; }

  void run(IRFunction* F);

  bool invalidate(IRFunction* F, unsigned Mask) override {
    return (Mask & Analysis::CFG) != 0;
  }

  /// Get dominator tree node for a block
  Node* getNode(IRBasicBlock* BB) const {
    auto It = Nodes.find(BB);
    return It != Nodes.end() ? It->second : nullptr;
  }

  /// Check if A dominates B
  bool dominates(IRBasicBlock* A, IRBasicBlock* B) const;

  /// Get immediate dominator
  IRBasicBlock* getIDom(IRBasicBlock* BB) const {
    Node* N = getNode(BB);
    return N && N->IDom ? N->IDom->Block : nullptr;
  }

  Node* getRoot() const { return Root; }

  void print() const;
};

/// Liveness - liveness analysis for values
class Liveness : public Analysis {
public:
  struct BlockInfo {
    std::set<IRValue*> LiveIn;
    std::set<IRValue*> LiveOut;
    std::set<IRValue*> Use;
    std::set<IRValue*> Def;
  };

private:
  std::map<IRBasicBlock*, BlockInfo> BlockLiveness;

public:
  std::string getName() const override { return "Liveness"; }

  void run(IRFunction* F);

  bool invalidate(IRFunction* F, unsigned Mask) override {
    return (Mask & (Analysis::CFG | Analysis::Instructions | Analysis::Values)) != 0;
  }

  const BlockInfo* getBlockInfo(IRBasicBlock* BB) const {
    auto It = BlockLiveness.find(BB);
    return It != BlockLiveness.end() ? &It->second : nullptr;
  }

  bool isLiveAt(IRValue* V, IRBasicBlock* BB) const {
    auto Info = getBlockInfo(BB);
    return Info && Info->LiveIn.count(V);
  }
};

} // namespace yac

#endif // YAC_CODEGEN_PASS_H
