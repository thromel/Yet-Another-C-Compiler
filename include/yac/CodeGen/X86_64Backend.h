#ifndef YAC_CODEGEN_X86_64BACKEND_H
#define YAC_CODEGEN_X86_64BACKEND_H

#include "yac/CodeGen/IR.h"
#include "yac/CodeGen/RegisterAllocator.h"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace yac {

/// X86_64Backend - generates x86-64 assembly from IR
/// Uses linear scan register allocation
class X86_64Backend {
public:
  X86_64Backend(std::ostream& Out) : OS(Out) {}

  /// Generate assembly for a module
  void generateAssembly(IRModule* M);

  /// Generate assembly for a function
  void generateFunction(IRFunction* F);

private:
  std::ostream& OS;
  RegisterAllocator* RegAlloc = nullptr;  // Current function's allocator
  std::map<std::string, IRBasicBlock*> LabelToBlock;  // Maps label names to blocks

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

  // Helper methods
  std::string getOperand(IRValue* V);
  std::string allocResult(IRValue* V);  // Allocate register for result
  std::string getByteReg(const std::string& Reg);
  void emitPrologue(IRFunction* F);
  void emitEpilogue(IRFunction* F);
  void emitPhiMoves(IRBasicBlock* FromBB, IRBasicBlock* ToBB);
  void loadSpilledValue(IRValue* V, const std::string& TempReg);
  void storeSpilledValue(IRValue* V, const std::string& TempReg);
};

} // namespace yac

#endif // YAC_CODEGEN_X86_64BACKEND_H
