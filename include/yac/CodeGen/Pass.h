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

/// Loop - represents a natural loop in the CFG
class Loop {
public:
  Loop(IRBasicBlock* Header) : Header(Header), ParentLoop(nullptr) {}

  IRBasicBlock* getHeader() const { return Header; }
  Loop* getParentLoop() const { return ParentLoop; }
  void setParentLoop(Loop* P) { ParentLoop = P; }

  // Blocks in the loop
  void addBlock(IRBasicBlock* BB) { Blocks.insert(BB); }
  bool contains(IRBasicBlock* BB) const { return Blocks.count(BB) > 0; }
  const std::set<IRBasicBlock*>& getBlocks() const { return Blocks; }

  // Sub-loops
  void addSubLoop(Loop* SubLoop) { SubLoops.push_back(SubLoop); }
  const std::vector<Loop*>& getSubLoops() const { return SubLoops; }

  // Latches (blocks with back-edges to header)
  void addLatch(IRBasicBlock* BB) { Latches.push_back(BB); }
  const std::vector<IRBasicBlock*>& getLatches() const { return Latches; }

  // Preheader (single predecessor outside loop)
  void setPreheader(IRBasicBlock* BB) { Preheader = BB; }
  IRBasicBlock* getPreheader() const { return Preheader; }

  // Depth
  unsigned getLoopDepth() const {
    unsigned D = 1;
    for (const Loop* P = ParentLoop; P; P = P->ParentLoop) {
      ++D;
    }
    return D;
  }

private:
  IRBasicBlock* Header;
  IRBasicBlock* Preheader = nullptr;
  Loop* ParentLoop;
  std::set<IRBasicBlock*> Blocks;
  std::vector<Loop*> SubLoops;
  std::vector<IRBasicBlock*> Latches;
};

/// LoopInfo - identifies and analyzes loops in a function
class LoopInfo : public Analysis {
public:
  std::string getName() const override { return "LoopInfo"; }

  void run(IRFunction* F);

  bool invalidate(IRFunction* F, unsigned Mask) override {
    return (Mask & (Analysis::CFG | Analysis::Instructions)) != 0;
  }

  // Query loops
  Loop* getLoopFor(IRBasicBlock* BB) const {
    auto It = BlockToLoop.find(BB);
    return It != BlockToLoop.end() ? It->second : nullptr;
  }

  const std::vector<std::unique_ptr<Loop>>& getTopLevelLoops() const {
    return TopLevelLoops;
  }

  unsigned getLoopDepth(IRBasicBlock* BB) const {
    Loop* L = getLoopFor(BB);
    return L ? L->getLoopDepth() : 0;
  }

  // Helpers
  bool isLoopHeader(IRBasicBlock* BB) const {
    Loop* L = getLoopFor(BB);
    return L && L->getHeader() == BB;
  }

private:
  std::vector<std::unique_ptr<Loop>> TopLevelLoops;
  std::map<IRBasicBlock*, Loop*> BlockToLoop;

  // Analysis helpers
  void identifyLoops(IRFunction* F, DominatorTree* DT);
  Loop* createLoop(IRBasicBlock* Header);
  void populateLoop(Loop* L, IRBasicBlock* BB, const std::set<IRBasicBlock*>& BackEdgeSources);
};

} // namespace yac

#endif // YAC_CODEGEN_PASS_H
