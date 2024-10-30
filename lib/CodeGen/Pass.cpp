#include "yac/CodeGen/Pass.h"
#include "yac/CodeGen/IRVerifier.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>

namespace yac {

// ===----------------------------------------------------------------------===
// PassManager
// ===----------------------------------------------------------------------===

bool PassManager::run(IRFunction* F) {
  AnalysisManager AM(F);
  bool Changed = false;

  for (auto& P : Passes) {
    std::cout << "Running pass: " << P->getName() << "\n";

    // Count instructions before
    size_t InstrsBefore = EnableTiming ? countInstructions(F) : 0;

    // Time the pass
    auto Start = std::chrono::high_resolution_clock::now();
    bool PassChanged = P->run(F, AM);
    auto End = std::chrono::high_resolution_clock::now();

    Changed |= PassChanged;

    // Record stats
    if (EnableTiming) {
      double TimeMs = std::chrono::duration<double, std::milli>(End - Start).count();
      size_t InstrsAfter = countInstructions(F);
      Stats.push_back({P->getName(), TimeMs, InstrsBefore, InstrsAfter});
    }

    if (PassChanged) {
      // Invalidate analyses based on what the pass preserves
      unsigned InvalidMask = 0;
      if (!P->preservesCFG()) {
        InvalidMask |= Analysis::CFG;
      }
      if (!P->preservesInstructions()) {
        InvalidMask |= Analysis::Instructions | Analysis::Values;
      }
      AM.invalidate(InvalidMask);

      // Verify after each pass if requested
      if (VerifyEach) {
        IRVerifier V(false);
        if (!V.verifyFunction(F)) {
          std::cerr << "Verification failed after pass: " << P->getName() << "\n";
          V.printErrors();
          return false;
        }
      }
    }
  }

  return Changed;
}

bool PassManager::run(IRModule* M) {
  bool Changed = false;

  for (auto& F : M->getFunctions()) {
    if (run(F.get())) {
      Changed = true;
    }
  }

  return Changed;
}

// ===----------------------------------------------------------------------===
// DominatorTree
// ===----------------------------------------------------------------------===

void DominatorTree::run(IRFunction* F) {
  AllNodes.clear();
  Nodes.clear();
  Root = nullptr;

  if (F->getBlocks().empty()) return;

  // Create nodes for all blocks
  for (const auto& BB : F->getBlocks()) {
    auto N = std::make_unique<Node>();
    N->Block = BB.get();
    Nodes[BB.get()] = N.get();
    AllNodes.push_back(std::move(N));
  }

  // Entry block is the root and dominates itself
  IRBasicBlock* Entry = F->getBlocks()[0].get();
  Root = Nodes[Entry];
  Root->IDom = nullptr;

  // Compute dominators using iterative algorithm
  // Initialize: all blocks except entry are dominated by all blocks
  std::map<IRBasicBlock*, std::set<IRBasicBlock*>> Doms;

  for (const auto& BB : F->getBlocks()) {
    if (BB.get() == Entry) {
      Doms[Entry].insert(Entry);
    } else {
      for (const auto& BB2 : F->getBlocks()) {
        Doms[BB.get()].insert(BB2.get());
      }
    }
  }

  // Iterate until fixpoint
  bool Changed = true;
  while (Changed) {
    Changed = false;

    for (const auto& BB : F->getBlocks()) {
      if (BB.get() == Entry) continue;

      std::set<IRBasicBlock*> NewDoms;
      NewDoms.insert(BB.get());

      // Intersect dominators of all predecessors
      bool First = true;
      for (IRBasicBlock* Pred : BB->getPredecessors()) {
        if (First) {
          NewDoms.insert(Doms[Pred].begin(), Doms[Pred].end());
          First = false;
        } else {
          std::set<IRBasicBlock*> Intersection;
          std::set_intersection(
            NewDoms.begin(), NewDoms.end(),
            Doms[Pred].begin(), Doms[Pred].end(),
            std::inserter(Intersection, Intersection.begin())
          );
          NewDoms = std::move(Intersection);
          NewDoms.insert(BB.get());
        }
      }

      if (NewDoms != Doms[BB.get()]) {
        Doms[BB.get()] = NewDoms;
        Changed = true;
      }
    }
  }

  // Compute immediate dominators
  for (const auto& BB : F->getBlocks()) {
    if (BB.get() == Entry) continue;

    IRBasicBlock* IDom = nullptr;

    // IDom is the unique dominator that is dominated by all other dominators
    for (IRBasicBlock* D : Doms[BB.get()]) {
      if (D == BB.get()) continue;

      bool IsIDom = true;
      for (IRBasicBlock* D2 : Doms[BB.get()]) {
        if (D2 == BB.get() || D2 == D) continue;
        if (Doms[D].count(D2) == 0) {
          IsIDom = false;
          break;
        }
      }

      if (IsIDom) {
        IDom = D;
        break;
      }
    }

    if (IDom) {
      Node* N = Nodes[BB.get()];
      N->IDom = Nodes[IDom];
      Nodes[IDom]->Children.push_back(N);
    }
  }
}

bool DominatorTree::dominates(IRBasicBlock* A, IRBasicBlock* B) const {
  if (A == B) return true;

  Node* NA = getNode(A);
  Node* NB = getNode(B);
  if (!NA || !NB) return false;

  // Walk up B's dominator tree
  while (NB) {
    if (NB == NA) return true;
    NB = NB->IDom;
  }

  return false;
}

void DominatorTree::print() const {
  std::cout << "Dominator Tree:\n";
  if (!Root) {
    std::cout << "  (empty)\n";
    return;
  }

  std::function<void(Node*, int)> PrintNode = [&](Node* N, int Depth) {
    for (int i = 0; i < Depth; ++i) std::cout << "  ";
    std::cout << N->Block->getName() << "\n";
    for (Node* Child : N->Children) {
      PrintNode(Child, Depth + 1);
    }
  };

  PrintNode(Root, 0);
}

// ===----------------------------------------------------------------------===
// Liveness
// ===----------------------------------------------------------------------===

void Liveness::run(IRFunction* F) {
  BlockLiveness.clear();

  // Initialize Use and Def sets for each block
  for (const auto& BB : F->getBlocks()) {
    BlockInfo& Info = BlockLiveness[BB.get()];

    for (const auto& Inst : BB->getInstructions()) {
      // Simplified: only handle binary instructions for now
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        // If operand is used before being defined in this block, add to Use
        if (!Info.Def.count(BinOp->getLHS()) && !BinOp->getLHS()->isConstant()) {
          Info.Use.insert(BinOp->getLHS());
        }
        if (!Info.Def.count(BinOp->getRHS()) && !BinOp->getRHS()->isConstant()) {
          Info.Use.insert(BinOp->getRHS());
        }

        // Result is defined
        Info.Def.insert(BinOp->getResult());
      }
      // TODO: Handle other instruction types
    }
  }

  // Iterate to compute LiveIn and LiveOut
  bool Changed = true;
  while (Changed) {
    Changed = false;

    for (const auto& BB : F->getBlocks()) {
      BlockInfo& Info = BlockLiveness[BB.get()];

      // LiveOut = Union of LiveIn of successors
      std::set<IRValue*> NewLiveOut;
      for (IRBasicBlock* Succ : BB->getSuccessors()) {
        const BlockInfo& SuccInfo = BlockLiveness[Succ];
        NewLiveOut.insert(SuccInfo.LiveIn.begin(), SuccInfo.LiveIn.end());
      }

      // LiveIn = Use ∪ (LiveOut - Def)
      std::set<IRValue*> NewLiveIn = Info.Use;
      for (IRValue* V : NewLiveOut) {
        if (!Info.Def.count(V)) {
          NewLiveIn.insert(V);
        }
      }

      if (NewLiveIn != Info.LiveIn || NewLiveOut != Info.LiveOut) {
        Info.LiveIn = NewLiveIn;
        Info.LiveOut = NewLiveOut;
        Changed = true;
      }
    }
  }
}

// ===----------------------------------------------------------------------===
// LoopInfo Analysis
// ===----------------------------------------------------------------------===

void LoopInfo::run(IRFunction* F) {
  TopLevelLoops.clear();
  BlockToLoop.clear();

  // Need dominator tree to identify back-edges
  DominatorTree DT;
  DT.run(F);

  identifyLoops(F, &DT);
}

void LoopInfo::identifyLoops(IRFunction* F, DominatorTree* DT) {
  // Find all back-edges (edges where target dominates source)
  std::map<IRBasicBlock*, std::set<IRBasicBlock*>> BackEdges;

  for (const auto& BB : F->getBlocks()) {
    for (IRBasicBlock* Succ : BB->getSuccessors()) {
      // Check if Succ dominates BB (back-edge)
      if (DT->dominates(Succ, BB.get())) {
        BackEdges[Succ].insert(BB.get());
      }
    }
  }

  // For each back-edge target (loop header), create a loop
  for (const auto& Entry : BackEdges) {
    IRBasicBlock* Header = Entry.first;
    const std::set<IRBasicBlock*>& Sources = Entry.second;

    Loop* L = createLoop(Header);

    // Add header to loop
    L->addBlock(Header);

    // Add all blocks that can reach a back-edge source without going through header
    for (IRBasicBlock* Source : Sources) {
      L->addLatch(Source);
      populateLoop(L, Source, Sources);
    }

    // Try to identify/create preheader
    // A preheader is a single predecessor of the header that's outside the loop
    std::vector<IRBasicBlock*> OutsidePreds;
    for (IRBasicBlock* Pred : Header->getPredecessors()) {
      if (!L->contains(Pred)) {
        OutsidePreds.push_back(Pred);
      }
    }

    if (OutsidePreds.size() == 1) {
      L->setPreheader(OutsidePreds[0]);
    }
  }
}

Loop* LoopInfo::createLoop(IRBasicBlock* Header) {
  auto L = std::make_unique<Loop>(Header);
  Loop* LPtr = L.get();

  TopLevelLoops.push_back(std::move(L));
  BlockToLoop[Header] = LPtr;

  return LPtr;
}

void LoopInfo::populateLoop(Loop* L, IRBasicBlock* BB,
                             const std::set<IRBasicBlock*>& BackEdgeSources) {
  // Add BB and all blocks that can reach it without going through the header
  if (BB == L->getHeader()) {
    return;  // Don't go past header
  }

  if (L->contains(BB)) {
    return;  // Already added
  }

  L->addBlock(BB);
  BlockToLoop[BB] = L;

  // Recursively add predecessors
  for (IRBasicBlock* Pred : BB->getPredecessors()) {
    populateLoop(L, Pred, BackEdgeSources);
  }
}

// ===----------------------------------------------------------------------===
// PassManager helper methods
// ===----------------------------------------------------------------------===

size_t PassManager::countInstructions(IRFunction* F) const {
  size_t Count = 0;
  for (const auto& BB : F->getBlocks()) {
    Count += BB->getInstructions().size();
  }
  return Count;
}

void PassManager::printTimingReport() const {
  if (Stats.empty()) {
    return;
  }

  std::cout << "\n=== Timing Report ===\n\n";
  std::cout << std::left;
  std::cout << std::setw(20) << "Pass Name"
            << std::setw(12) << "Time (ms)"
            << std::setw(12) << "Before"
            << std::setw(12) << "After"
            << std::setw(12) << "Delta"
            << "\n";
  std::cout << std::string(68, '-') << "\n";

  double TotalTime = 0.0;
  for (const auto& S : Stats) {
    int Delta = static_cast<int>(S.InstructionsAfter) - static_cast<int>(S.InstructionsBefore);
    std::cout << std::setw(20) << S.Name
              << std::setw(12) << std::fixed << std::setprecision(3) << S.TimeMs
              << std::setw(12) << S.InstructionsBefore
              << std::setw(12) << S.InstructionsAfter
              << std::setw(12) << (Delta >= 0 ? "+" : "") << Delta
              << "\n";
    TotalTime += S.TimeMs;
  }

  std::cout << std::string(68, '-') << "\n";
  std::cout << std::setw(20) << "Total"
            << std::setw(12) << std::fixed << std::setprecision(3) << TotalTime
            << "\n\n";
}

} // namespace yac
