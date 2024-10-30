#include "yac/CodeGen/X86_64Backend.h"
#include <iomanip>

namespace yac {

void X86_64Backend::generateAssembly(IRModule* M) {
  // Emit assembly header
  OS << "\t.text\n";
  OS << "\t.intel_syntax noprefix\n\n";

  // Generate code for each function
  for (const auto& F : M->getFunctions()) {
    generateFunction(F.get());
    OS << "\n";
  }
}

void X86_64Backend::generateFunction(IRFunction* F) {
  // Reset state for new function
  ValueToReg.clear();
  UsedRegs.clear();
  StackOffset = 0;
  LabelToBlock.clear();

  // Build label to block mapping
  for (const auto& BB : F->getBlocks()) {
    LabelToBlock[BB->getName()] = BB.get();
  }

  // Function label (make main global)
  if (F->getName() == "main") {
    OS << "\t.globl _main\n";
    OS << "_main:\n";
  } else {
    OS << "\t.globl _" << F->getName() << "\n";
    OS << "_" << F->getName() << ":\n";
  }

  emitPrologue(F);

  // Generate code for each basic block
  for (const auto& BB : F->getBlocks()) {
    // Skip entry block label (already at function start)
    if (BB->getName() != "entry") {
      OS << "." << BB->getName() << ":\n";
    }

    // Generate instructions
    for (const auto& Inst : BB->getInstructions()) {
      generateInstruction(Inst.get());
    }
  }

  emitEpilogue(F);
}

void X86_64Backend::emitPrologue(IRFunction* F) {
  OS << "\tpush rbp\n";
  OS << "\tmov rbp, rsp\n";

  // Reserve stack space for locals (simplified - allocate 128 bytes)
  OS << "\tsub rsp, 128\n";
}

void X86_64Backend::emitEpilogue(IRFunction* F) {
  // Note: epilogue is emitted in ret instruction
}

void X86_64Backend::generateInstruction(IRInstruction* I) {
  if (auto* BinOp = dynamic_cast<IRBinaryInst*>(I)) {
    generateBinaryInst(BinOp);
  } else if (auto* UnOp = dynamic_cast<IRUnaryInst*>(I)) {
    generateUnaryInst(UnOp);
  } else if (auto* Load = dynamic_cast<IRLoadInst*>(I)) {
    generateLoadInst(Load);
  } else if (auto* Store = dynamic_cast<IRStoreInst*>(I)) {
    generateStoreInst(Store);
  } else if (auto* Alloca = dynamic_cast<IRAllocaInst*>(I)) {
    generateAllocaInst(Alloca);
  } else if (auto* Ret = dynamic_cast<IRRetInst*>(I)) {
    generateRetInst(Ret);
  } else if (auto* Br = dynamic_cast<IRBrInst*>(I)) {
    generateBrInst(Br);
  } else if (auto* CondBr = dynamic_cast<IRCondBrInst*>(I)) {
    generateCondBrInst(CondBr);
  } else if (auto* Call = dynamic_cast<IRCallInst*>(I)) {
    generateCallInst(Call);
  } else if (auto* Phi = dynamic_cast<IRPhiInst*>(I)) {
    generatePhiInst(Phi);
  }
}

void X86_64Backend::generateBinaryInst(IRBinaryInst* I) {
  std::string lhs = getOperand(I->getLHS());
  std::string rhs = getOperand(I->getRHS());
  std::string result = allocateRegister(I->getResult());

  switch (I->getOpcode()) {
  case IRInstruction::Add:
    OS << "\tmov " << result << ", " << lhs << "\n";
    OS << "\tadd " << result << ", " << rhs << "\n";
    break;
  case IRInstruction::Sub:
    OS << "\tmov " << result << ", " << lhs << "\n";
    OS << "\tsub " << result << ", " << rhs << "\n";
    break;
  case IRInstruction::Mul:
    OS << "\tmov rax, " << lhs << "\n";
    OS << "\timul " << rhs << "\n";
    OS << "\tmov " << result << ", rax\n";
    break;
  case IRInstruction::Div:
    OS << "\tmov rax, " << lhs << "\n";
    OS << "\tcqo\n";  // Sign extend
    OS << "\tidiv " << rhs << "\n";
    OS << "\tmov " << result << ", rax\n";
    break;
  case IRInstruction::Lt:
    OS << "\txor " << result << ", " << result << "\n";
    OS << "\tcmp " << lhs << ", " << rhs << "\n";
    OS << "\tsetl " << getByteReg(result) << "\n";
    break;
  case IRInstruction::Le:
    OS << "\txor " << result << ", " << result << "\n";
    OS << "\tcmp " << lhs << ", " << rhs << "\n";
    OS << "\tsetle " << getByteReg(result) << "\n";
    break;
  case IRInstruction::Gt:
    OS << "\txor " << result << ", " << result << "\n";
    OS << "\tcmp " << lhs << ", " << rhs << "\n";
    OS << "\tsetg " << getByteReg(result) << "\n";
    break;
  case IRInstruction::Ge:
    OS << "\txor " << result << ", " << result << "\n";
    OS << "\tcmp " << lhs << ", " << rhs << "\n";
    OS << "\tsetge " << getByteReg(result) << "\n";
    break;
  case IRInstruction::Eq:
    OS << "\txor " << result << ", " << result << "\n";
    OS << "\tcmp " << lhs << ", " << rhs << "\n";
    OS << "\tsete " << getByteReg(result) << "\n";
    break;
  case IRInstruction::Ne:
    OS << "\txor " << result << ", " << result << "\n";
    OS << "\tcmp " << lhs << ", " << rhs << "\n";
    OS << "\tsetne " << getByteReg(result) << "\n";
    break;
  default:
    OS << "\t# TODO: " << IRInstruction::getOpcodeName(I->getOpcode()) << "\n";
    break;
  }
}

void X86_64Backend::generateUnaryInst(IRUnaryInst* I) {
  std::string operand = getOperand(I->getOperand());
  std::string result = allocateRegister(I->getResult());

  switch (I->getOpcode()) {
  case IRInstruction::Not:
    OS << "\tmov " << result << ", " << operand << "\n";
    OS << "\txor " << result << ", 1\n";  // Logical not: 0->1, non-zero->0
    break;
  default:
    OS << "\t# TODO: Unary " << IRInstruction::getOpcodeName(I->getOpcode()) << "\n";
    break;
  }
}

void X86_64Backend::generateLoadInst(IRLoadInst* I) {
  // Simplified: ignore for SSA form (loads eliminated by Mem2Reg)
  OS << "\t# load eliminated by SSA\n";
}

void X86_64Backend::generateStoreInst(IRStoreInst* I) {
  // Simplified: ignore for SSA form (stores eliminated by Mem2Reg)
  OS << "\t# store eliminated by SSA\n";
}

void X86_64Backend::generateAllocaInst(IRAllocaInst* I) {
  // Simplified: allocas handled by stack frame
  OS << "\t# alloca handled in prologue\n";
}

void X86_64Backend::generateRetInst(IRRetInst* I) {
  if (I->hasRetValue()) {
    std::string retVal = getOperand(I->getRetValue());
    OS << "\tmov rax, " << retVal << "\n";
  }

  // Epilogue
  OS << "\tmov rsp, rbp\n";
  OS << "\tpop rbp\n";
  OS << "\tret\n";
}

void X86_64Backend::generateBrInst(IRBrInst* I) {
  IRBasicBlock* FromBB = I->getParent();
  std::string targetName = I->getTarget()->getName();
  IRBasicBlock* ToBB = LabelToBlock[targetName];

  // Emit phi moves before jumping
  if (ToBB) {
    emitPhiMoves(FromBB, ToBB);
  }

  OS << "\tjmp ." << targetName << "\n";
}

void X86_64Backend::generateCondBrInst(IRCondBrInst* I) {
  IRBasicBlock* FromBB = I->getParent();
  std::string trueName = I->getTrueLabel()->getName();
  std::string falseName = I->getFalseLabel()->getName();
  IRBasicBlock* TrueBB = LabelToBlock[trueName];
  IRBasicBlock* FalseBB = LabelToBlock[falseName];

  std::string cond = getOperand(I->getCondition());

  OS << "\ttest " << cond << ", " << cond << "\n";

  // Emit phi moves for true branch, then jump
  OS << "\tjz .false_branch_" << FromBB->getName() << "\n";
  if (TrueBB) {
    emitPhiMoves(FromBB, TrueBB);
  }
  OS << "\tjmp ." << trueName << "\n";

  // False branch
  OS << ".false_branch_" << FromBB->getName() << ":\n";
  if (FalseBB) {
    emitPhiMoves(FromBB, FalseBB);
  }
  OS << "\tjmp ." << falseName << "\n";
}

void X86_64Backend::generateCallInst(IRCallInst* I) {
  // Simplified calling convention (System V AMD64 ABI)
  const auto& Args = I->getArgs();
  const char* ArgRegs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

  // Move arguments to registers
  for (size_t i = 0; i < Args.size() && i < 6; ++i) {
    std::string arg = getOperand(Args[i]);
    OS << "\tmov " << ArgRegs[i] << ", " << arg << "\n";
  }

  // Call function (add underscore for macOS)
  OS << "\tcall _" << I->getFuncName() << "\n";

  // Result in rax
  if (I->getResult()) {
    std::string result = allocateRegister(I->getResult());
    OS << "\tmov " << result << ", rax\n";
  }
}

void X86_64Backend::generatePhiInst(IRPhiInst* I) {
  // Phi nodes handled by register allocation
  // For now, just allocate a register for the result
  allocateRegister(I->getResult());
  OS << "\t# phi node (handled by regalloc)\n";
}

std::string X86_64Backend::allocateRegister(IRValue* V) {
  // Check if already allocated
  if (ValueToReg.count(V)) {
    return ValueToReg[V];
  }

  // Find an unused register
  for (const auto& reg : AvailableRegs) {
    if (UsedRegs.find(reg) == UsedRegs.end()) {
      ValueToReg[V] = reg;
      UsedRegs.insert(reg);
      return reg;
    }
  }

  // If all registers used, just use r10 (will need spilling in real implementation)
  std::string reg = "r10";
  ValueToReg[V] = reg;
  return reg;
}

std::string X86_64Backend::getOperand(IRValue* V) {
  if (V->isConstant()) {
    return std::to_string(V->getConstant());
  }

  if (ValueToReg.count(V)) {
    return ValueToReg[V];
  }

  // Allocate register for this value
  return allocateRegister(V);
}

void X86_64Backend::freeRegister(const std::string& Reg) {
  UsedRegs.erase(Reg);
}

std::string X86_64Backend::getByteReg(const std::string& Reg) {
  // Convert 64-bit register to 8-bit register for setcc instructions
  if (Reg == "rax") return "al";
  if (Reg == "rbx") return "bl";
  if (Reg == "rcx") return "cl";
  if (Reg == "rdx") return "dl";
  if (Reg == "rsi") return "sil";
  if (Reg == "rdi") return "dil";
  if (Reg == "r8")  return "r8b";
  if (Reg == "r9")  return "r9b";
  if (Reg == "r10") return "r10b";
  if (Reg == "r11") return "r11b";
  if (Reg == "r12") return "r12b";
  if (Reg == "r13") return "r13b";
  if (Reg == "r14") return "r14b";
  if (Reg == "r15") return "r15b";
  return "al";  // Default
}

void X86_64Backend::emitPhiMoves(IRBasicBlock* FromBB, IRBasicBlock* ToBB) {
  // Emit moves for phi nodes in the target block
  // For each phi in ToBB, find the incoming value from FromBB and emit a mov

  for (const auto& Inst : ToBB->getInstructions()) {
    auto* Phi = dynamic_cast<IRPhiInst*>(Inst.get());
    if (!Phi) {
      // Phi nodes are at the beginning of blocks
      break;
    }

    // Find the incoming value from FromBB
    for (const auto& Entry : Phi->getIncomings()) {
      if (Entry.Block == FromBB) {
        // Found the incoming value for this predecessor
        std::string phiReg = allocateRegister(Phi->getResult());
        std::string valueOp = getOperand(Entry.Value);
        OS << "\tmov " << phiReg << ", " << valueOp << "\n";
        break;
      }
    }
  }
}

} // namespace yac
