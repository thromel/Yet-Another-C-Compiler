#ifndef YAC_CODEGEN_REGISTERALLOCATOR_H
#define YAC_CODEGEN_REGISTERALLOCATOR_H

#include "yac/CodeGen/IR.h"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace yac {

/// Live interval for a value
struct LiveInterval {
  IRValue* Value;
  int Start;  // First instruction index where value is live
  int End;    // Last instruction index where value is live

  LiveInterval(IRValue* V, int S, int E) : Value(V), Start(S), End(E) {}

  bool overlaps(const LiveInterval& Other) const {
    return !(End < Other.Start || Other.End < Start);
  }

  bool operator<(const LiveInterval& Other) const {
    return Start < Other.Start;
  }
};

/// Linear scan register allocator
class RegisterAllocator {
public:
  RegisterAllocator() {
    // Available physical registers (caller-saved for now)
    AvailableRegs = {
      "rax", "rcx", "rdx", "rsi", "rdi",
      "r8", "r9", "r10", "r11"
    };
  }

  /// Allocate registers for a function
  void allocate(IRFunction* F);

  /// Get the register assigned to a value
  std::string getRegister(IRValue* V) const;

  /// Check if a value is spilled to stack
  bool isSpilled(IRValue* V) const;

  /// Get stack offset for a spilled value
  int getStackOffset(IRValue* V) const;

  /// Get total stack space needed for spills
  int getSpillSlotSize() const { return SpillSlotCount * 8; }

private:
  std::vector<std::string> AvailableRegs;
  std::map<IRValue*, std::string> ValueToReg;     // Allocated registers
  std::map<IRValue*, int> ValueToStackSlot;       // Spilled values
  int SpillSlotCount = 0;

  // Live interval computation
  std::vector<LiveInterval> computeLiveIntervals(IRFunction* F);
  std::map<IRValue*, std::set<int>> computeLiveness(IRFunction* F);
  int getInstructionIndex(IRFunction* F, IRInstruction* I);

  // Allocation helpers
  void allocateInterval(LiveInterval& Interval,
                        std::vector<LiveInterval>& Active);
  void expireOldIntervals(int CurrentStart,
                          std::vector<LiveInterval>& Active,
                          std::set<std::string>& FreeRegs);
  void spillAtInterval(LiveInterval& Interval,
                       std::vector<LiveInterval>& Active);
};

} // namespace yac

#endif // YAC_CODEGEN_REGISTERALLOCATOR_H
