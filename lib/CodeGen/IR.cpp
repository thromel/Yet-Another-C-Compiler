#include "yac/CodeGen/IR.h"
#include <iostream>

namespace yac {

// ===----------------------------------------------------------------------===
// IRValue
// ===----------------------------------------------------------------------===

std::string IRValue::toString() const {
  if (isConstant()) {
    return std::to_string(ConstantValue);
  }

  switch (Kind) {
  case VK_Temp:
    return "%" + Name;
  case VK_Global:
    return "@" + Name;
  case VK_Local:
    return "%" + Name;
  case VK_Label:
    return Name;
  default:
    return Name;
  }
}

// ===----------------------------------------------------------------------===
// IRInstruction
// ===----------------------------------------------------------------------===

const char* IRInstruction::getOpcodeName(Opcode Op) {
  switch (Op) {
  case Add: return "add";
  case Sub: return "sub";
  case Mul: return "mul";
  case Div: return "div";
  case Mod: return "mod";
  case And: return "and";
  case Or: return "or";
  case Xor: return "xor";
  case Shl: return "shl";
  case Shr: return "shr";
  case Not: return "not";
  case Eq: return "eq";
  case Ne: return "ne";
  case Lt: return "lt";
  case Le: return "le";
  case Gt: return "gt";
  case Ge: return "ge";
  case Load: return "load";
  case Store: return "store";
  case Alloca: return "alloca";
  case Br: return "br";
  case CondBr: return "condbr";
  case Ret: return "ret";
  case Call: return "call";
  case IntToFloat: return "itof";
  case FloatToInt: return "ftoi";
  case Move: return "move";
  case Label: return "label";
  case Phi: return "phi";
  default: return "unknown";
  }
}

std::string IRBinaryInst::toString() const {
  return Result->toString() + " = " + getOpcodeName(getOpcode()) + " " +
         LHS->toString() + ", " + RHS->toString();
}

std::string IRUnaryInst::toString() const {
  return Result->toString() + " = " + getOpcodeName(getOpcode()) + " " +
         Operand->toString();
}

std::string IRLoadInst::toString() const {
  return Result->toString() + " = load " + Ptr->toString();
}

std::string IRStoreInst::toString() const {
  return "store " + Value->toString() + ", " + Ptr->toString();
}

std::string IRAllocaInst::toString() const {
  return Result->toString() + " = alloca " + AllocType->toString();
}

std::string IRRetInst::toString() const {
  if (hasRetValue()) {
    return "ret " + RetValue->toString();
  }
  return "ret";
}

std::string IRBrInst::toString() const {
  return "br " + Target->toString();
}

std::string IRCondBrInst::toString() const {
  return "br " + Condition->toString() + ", " +
         TrueLabel->toString() + ", " + FalseLabel->toString();
}

std::string IRCallInst::toString() const {
  std::string Str;
  if (Result) {
    Str = Result->toString() + " = ";
  }
  Str += "call " + FuncName + "(";
  for (size_t i = 0; i < Args.size(); ++i) {
    if (i > 0) Str += ", ";
    Str += Args[i]->toString();
  }
  Str += ")";
  return Str;
}

std::string IRLabelInst::toString() const {
  return Label->toString() + ":";
}

std::string IRMoveInst::toString() const {
  return Result->toString() + " = " + Operand->toString();
}

std::string IRPhiInst::toString() const {
  std::string Str = Result->toString() + " = phi ";
  for (size_t i = 0; i < Incomings.size(); ++i) {
    if (i > 0) Str += ", ";
    Str += "[" + Incomings[i].Value->toString() + ", " +
           Incomings[i].Block->getName() + "]";
  }
  return Str;
}

// ===----------------------------------------------------------------------===
// IRBasicBlock
// ===----------------------------------------------------------------------===

void IRBasicBlock::print() const {
  std::cout << Name << ":\n";
  for (const auto& Inst : Instructions) {
    std::cout << "  " << Inst->toString() << "\n";
  }
}

// ===----------------------------------------------------------------------===
// IRFunction
// ===----------------------------------------------------------------------===

void IRFunction::print() const {
  std::cout << "\nfunction " << Name << "(";
  for (size_t i = 0; i < Parameters.size(); ++i) {
    if (i > 0) std::cout << ", ";
    std::cout << Parameters[i]->toString() << ": " << Parameters[i]->getType()->toString();
  }
  std::cout << ") -> " << ReturnType->toString() << " {\n";

  for (const auto& Block : Blocks) {
    Block->print();
  }

  std::cout << "}\n";
}

// ===----------------------------------------------------------------------===
// IRModule
// ===----------------------------------------------------------------------===

void IRModule::print() const {
  std::cout << "=== IR Module ===\n";

  if (!GlobalValues.empty()) {
    std::cout << "\nGlobals:\n";
    for (const auto& Global : GlobalValues) {
      std::cout << "  " << Global->toString() << ": " << Global->getType()->toString() << "\n";
    }
  }

  for (const auto& Func : Functions) {
    Func->print();
  }
}

} // namespace yac
