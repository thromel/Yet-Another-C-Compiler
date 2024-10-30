#ifndef YAC_CODEGEN_X86_64BACKEND_H
#define YAC_CODEGEN_X86_64BACKEND_H

#include "yac/CodeGen/IR.h"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace yac {

/// X86_64Backend - generates x86-64 assembly from IR
/// Simplified backend with basic register allocation
class X86_64Backend {
public:
  X86_64Backend(std::ostream& Out) : OS(Out) {}

  /// Generate assembly for a module
  void generateAssembly(IRModule* M);

  /// Generate assembly for a function
  void generateFunction(IRFunction* F);

private:
  std::ostream& OS;

  // Register allocation state
  std::map<IRValue*, std::string> ValueToReg;  // Maps SSA values to registers
  std::set<std::string> UsedRegs;              // Currently allocated registers
  int StackOffset = 0;                         // Current stack offset
  std::map<std::string, IRBasicBlock*> LabelToBlock;  // Maps label names to blocks

  // Available registers (caller-saved only to avoid ABI violations)
  // Callee-saved registers (rbx, r12-r15) excluded - would need to be saved/restored
  std::vector<std::string> AvailableRegs = {
    "rax", "rcx", "rdx", "rsi", "rdi",
    "r8", "r9", "r10", "r11"
  };

  // Instruction generation
  void generateInstruction(IRInstruction* I);
  void generateBinaryInst(IRBinaryInst* I);
  void generateUnaryInst(IRUnaryInst* I);
  void generateLoadInst(IRLoadInst* I);
  void generateStoreInst(IRStoreInst* I);
  void generateAllocaInst(IRAllocaInst* I);
  void generateRetInst(IRRetInst* I);
  void generateBrInst(IRBrInst* I);
  void generateCondBrInst(IRCondBrInst* I);
  void generateCallInst(IRCallInst* I);
  void generatePhiInst(IRPhiInst* I);

  // Register management
  std::string allocateRegister(IRValue* V);
  std::string getOperand(IRValue* V);
  void freeRegister(const std::string& Reg);

  // Helper methods
  std::string getOpcodeName(IRInstruction::Opcode Op);
  std::string getByteReg(const std::string& Reg);
  void emitPrologue(IRFunction* F);
  void emitEpilogue(IRFunction* F);
  void emitPhiMoves(IRBasicBlock* FromBB, IRBasicBlock* ToBB);
};

} // namespace yac

#endif // YAC_CODEGEN_X86_64BACKEND_H
