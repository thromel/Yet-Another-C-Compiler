#include "yac/CodeGen/RegisterAllocator.h"
#include <algorithm>
#include <iostream>

namespace yac {

void RegisterAllocator::allocate(IRFunction* F) {
  // Compute live intervals
  auto Intervals = computeLiveIntervals(F);

  // Sort by start point (required for linear scan)
  std::sort(Intervals.begin(), Intervals.end());

  // Active intervals (currently allocated)
  std::vector<LiveInterval> Active;

  // Free registers
  std::set<std::string> FreeRegs(AvailableRegs.begin(), AvailableRegs.end());

  // Linear scan allocation
  for (auto& Interval : Intervals) {
    // Expire old intervals
    expireOldIntervals(Interval.Start, Active, FreeRegs);

    // Try to allocate a register
    if (!FreeRegs.empty()) {
      // Allocate the first available register
      std::string Reg = *FreeRegs.begin();
      FreeRegs.erase(FreeRegs.begin());
      ValueToReg[Interval.Value] = Reg;
      Active.push_back(Interval);
    } else {
      // No free registers, need to spill
      spillAtInterval(Interval, Active);
    }
  }
}

std::string RegisterAllocator::getRegister(IRValue* V) const {
  auto It = ValueToReg.find(V);
  if (It != ValueToReg.end()) {
    return It->second;
  }
  return "";  // Not allocated or spilled
}

bool RegisterAllocator::isSpilled(IRValue* V) const {
  return ValueToStackSlot.count(V) > 0;
}

int RegisterAllocator::getStackOffset(IRValue* V) const {
  auto It = ValueToStackSlot.find(V);
  if (It != ValueToStackSlot.end()) {
    return It->second;
  }
  return -1;
}

std::map<IRValue*, std::set<int>> RegisterAllocator::computeLiveness(IRFunction* F) {
  std::map<IRValue*, std::set<int>> LiveRanges;

  int InstIndex = 0;
  for (const auto& BB : F->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      // Mark uses and defs based on instruction type
      if (auto* BinOp = dynamic_cast<IRBinaryInst*>(Inst.get())) {
        if (BinOp->getLHS() && !BinOp->getLHS()->isConstant())
          LiveRanges[BinOp->getLHS()].insert(InstIndex);
        if (BinOp->getRHS() && !BinOp->getRHS()->isConstant())
          LiveRanges[BinOp->getRHS()].insert(InstIndex);
        if (BinOp->getResult() && !BinOp->getResult()->isConstant())
          LiveRanges[BinOp->getResult()].insert(InstIndex);
      } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(Inst.get())) {
        if (UnOp->getOperand() && !UnOp->getOperand()->isConstant())
          LiveRanges[UnOp->getOperand()].insert(InstIndex);
        if (UnOp->getResult() && !UnOp->getResult()->isConstant())
          LiveRanges[UnOp->getResult()].insert(InstIndex);
      } else if (auto* Load = dynamic_cast<IRLoadInst*>(Inst.get())) {
        if (Load->getPtr() && !Load->getPtr()->isConstant())
          LiveRanges[Load->getPtr()].insert(InstIndex);
        if (Load->getResult() && !Load->getResult()->isConstant())
          LiveRanges[Load->getResult()].insert(InstIndex);
      } else if (auto* Store = dynamic_cast<IRStoreInst*>(Inst.get())) {
        if (Store->getValue() && !Store->getValue()->isConstant())
          LiveRanges[Store->getValue()].insert(InstIndex);
        if (Store->getPtr() && !Store->getPtr()->isConstant())
          LiveRanges[Store->getPtr()].insert(InstIndex);
      } else if (auto* Alloca = dynamic_cast<IRAllocaInst*>(Inst.get())) {
        if (Alloca->getResult() && !Alloca->getResult()->isConstant())
          LiveRanges[Alloca->getResult()].insert(InstIndex);
      } else if (auto* Ret = dynamic_cast<IRRetInst*>(Inst.get())) {
        if (Ret->hasRetValue() && !Ret->getRetValue()->isConstant())
          LiveRanges[Ret->getRetValue()].insert(InstIndex);
      } else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(Inst.get())) {
        if (CondBr->getCondition() && !CondBr->getCondition()->isConstant())
          LiveRanges[CondBr->getCondition()].insert(InstIndex);
      } else if (auto* Call = dynamic_cast<IRCallInst*>(Inst.get())) {
        for (IRValue* Arg : Call->getArgs()) {
          if (Arg && !Arg->isConstant())
            LiveRanges[Arg].insert(InstIndex);
        }
        if (Call->getResult() && !Call->getResult()->isConstant())
          LiveRanges[Call->getResult()].insert(InstIndex);
      } else if (auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get())) {
        for (const auto& Entry : Phi->getIncomings()) {
          if (Entry.Value && !Entry.Value->isConstant())
            LiveRanges[Entry.Value].insert(InstIndex);
        }
        if (Phi->getResult() && !Phi->getResult()->isConstant())
          LiveRanges[Phi->getResult()].insert(InstIndex);
      } else if (auto* Move = dynamic_cast<IRMoveInst*>(Inst.get())) {
        if (Move->getOperand() && !Move->getOperand()->isConstant())
          LiveRanges[Move->getOperand()].insert(InstIndex);
        if (Move->getResult() && !Move->getResult()->isConstant())
          LiveRanges[Move->getResult()].insert(InstIndex);
      }

      InstIndex++;
    }
  }

  return LiveRanges;
}

std::vector<LiveInterval> RegisterAllocator::computeLiveIntervals(IRFunction* F) {
  auto LiveRanges = computeLiveness(F);
  std::vector<LiveInterval> Intervals;

  for (const auto& Entry : LiveRanges) {
    IRValue* V = Entry.first;
    const auto& Ranges = Entry.second;

    if (!Ranges.empty()) {
      int Start = *Ranges.begin();
      int End = *Ranges.rbegin();
      Intervals.emplace_back(V, Start, End);
    }
  }

  return Intervals;
}

int RegisterAllocator::getInstructionIndex(IRFunction* F, IRInstruction* I) {
  int Index = 0;
  for (const auto& BB : F->getBlocks()) {
    for (const auto& Inst : BB->getInstructions()) {
      if (Inst.get() == I) {
        return Index;
      }
      Index++;
    }
  }
  return -1;
}

void RegisterAllocator::expireOldIntervals(int CurrentStart,
                                           std::vector<LiveInterval>& Active,
                                           std::set<std::string>& FreeRegs) {
  // Remove intervals that have ended
  auto It = Active.begin();
  while (It != Active.end()) {
    if (It->End < CurrentStart) {
      // This interval has ended, free its register
      std::string Reg = ValueToReg[It->Value];
      FreeRegs.insert(Reg);
      It = Active.erase(It);
    } else {
      ++It;
    }
  }
}

void RegisterAllocator::spillAtInterval(LiveInterval& Interval,
                                        std::vector<LiveInterval>& Active) {
  // Find the interval in Active that ends last
  auto SpillCandidate = std::max_element(Active.begin(), Active.end(),
    [](const LiveInterval& A, const LiveInterval& B) {
      return A.End < B.End;
    });

  if (SpillCandidate != Active.end() && SpillCandidate->End > Interval.End) {
    // Spill the candidate instead of current interval
    IRValue* SpillValue = SpillCandidate->Value;
    std::string Reg = ValueToReg[SpillValue];

    // Assign spill slot
    ValueToStackSlot[SpillValue] = SpillSlotCount++;
    ValueToReg.erase(SpillValue);

    // Give the register to current interval
    ValueToReg[Interval.Value] = Reg;

    // Replace spilled interval with current interval
    *SpillCandidate = Interval;
  } else {
    // Spill the current interval
    ValueToStackSlot[Interval.Value] = SpillSlotCount++;
  }
}

} // namespace yac
