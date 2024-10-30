#ifndef YAC_CODEGEN_IR_H
#define YAC_CODEGEN_IR_H

#include "yac/Type/Type.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace yac {

// Forward declarations
class IRValue;
class IRInstruction;
class IRBasicBlock;
class IRFunction;

/// IRValue - represents a value in IR (variable, temporary, constant)
class IRValue {
public:
  enum ValueKind {
    VK_Temp,      // Temporary value (%0, %1, etc.)
    VK_Global,    // Global variable
    VK_Local,     // Local variable
    VK_Constant,  // Integer/float constant
    VK_Label      // Label for jumps
  };

private:
  ValueKind Kind;
  std::string Name;
  Type* ValType;
  int64_t ConstantValue = 0;  // For constants

public:
  IRValue(ValueKind K, std::string Name, Type* Ty)
      : Kind(K), Name(std::move(Name)), ValType(Ty) {}

  IRValue(int64_t Val)  // Constant constructor
      : Kind(VK_Constant), ValType(nullptr), ConstantValue(Val) {}

  ValueKind getKind() const { return Kind; }
  const std::string& getName() const { return Name; }
  Type* getType() const { return ValType; }
  int64_t getConstant() const { return ConstantValue; }

  bool isTemp() const { return Kind == VK_Temp; }
  bool isGlobal() const { return Kind == VK_Global; }
  bool isLocal() const { return Kind == VK_Local; }
  bool isConstant() const { return Kind == VK_Constant; }
  bool isLabel() const { return Kind == VK_Label; }

  std::string toString() const;
};

/// IRInstruction - base class for IR instructions
class IRInstruction {
public:
  enum Opcode {
    // Arithmetic
    Add, Sub, Mul, Div, Mod,
    // Logical/Bitwise
    And, Or, Xor, Shl, Shr, Not,
    // Comparison (result is 0 or 1)
    Eq, Ne, Lt, Le, Gt, Ge,
    // Memory
    Load, Store, Alloca,
    // Control flow
    Br, CondBr, Ret, Call,
    // Type conversions
    IntToFloat, FloatToInt,
    // Other
    Move, Label, Phi
  };

private:
  Opcode Op;
  IRBasicBlock* ParentBlock = nullptr;

public:
  IRInstruction(Opcode Op) : Op(Op) {}
  virtual ~IRInstruction() = default;

  Opcode getOpcode() const { return Op; }
  virtual std::string toString() const = 0;

  // Parent block tracking
  IRBasicBlock* getParent() const { return ParentBlock; }
  void setParent(IRBasicBlock* BB) { ParentBlock = BB; }

  // Terminator queries
  bool isTerminator() const {
    return Op == Br || Op == CondBr || Op == Ret;
  }

  static const char* getOpcodeName(Opcode Op);
};

/// Binary operation: result = op lhs, rhs
class IRBinaryInst : public IRInstruction {
  IRValue* Result;
  IRValue* LHS;
  IRValue* RHS;

public:
  IRBinaryInst(Opcode Op, IRValue* Result, IRValue* LHS, IRValue* RHS)
      : IRInstruction(Op), Result(Result), LHS(LHS), RHS(RHS) {}

  IRValue* getResult() const { return Result; }
  IRValue* getLHS() const { return LHS; }
  IRValue* getRHS() const { return RHS; }

  // Setters for operand replacement
  void setLHS(IRValue* V) { LHS = V; }
  void setRHS(IRValue* V) { RHS = V; }

  std::string toString() const override;
};

/// Unary operation: result = op operand
class IRUnaryInst : public IRInstruction {
  IRValue* Result;
  IRValue* Operand;

public:
  IRUnaryInst(Opcode Op, IRValue* Result, IRValue* Operand)
      : IRInstruction(Op), Result(Result), Operand(Operand) {}

  IRValue* getResult() const { return Result; }
  IRValue* getOperand() const { return Operand; }

  void setOperand(IRValue* V) { Operand = V; }

  std::string toString() const override;
};

/// Load: result = load ptr
class IRLoadInst : public IRInstruction {
  IRValue* Result;
  IRValue* Ptr;

public:
  IRLoadInst(IRValue* Result, IRValue* Ptr)
      : IRInstruction(Load), Result(Result), Ptr(Ptr) {}

  IRValue* getResult() const { return Result; }
  IRValue* getPtr() const { return Ptr; }

  std::string toString() const override;
};

/// Store: store value, ptr
class IRStoreInst : public IRInstruction {
  IRValue* Value;
  IRValue* Ptr;

public:
  IRStoreInst(IRValue* Value, IRValue* Ptr)
      : IRInstruction(Store), Value(Value), Ptr(Ptr) {}

  IRValue* getValue() const { return Value; }
  IRValue* getPtr() const { return Ptr; }

  // Setters for operand replacement
  void setValue(IRValue* V) { Value = V; }
  void setPtr(IRValue* P) { Ptr = P; }

  std::string toString() const override;
};

/// Alloca: result = alloca type
class IRAllocaInst : public IRInstruction {
  IRValue* Result;
  Type* AllocType;

public:
  IRAllocaInst(IRValue* Result, Type* Ty)
      : IRInstruction(Alloca), Result(Result), AllocType(Ty) {}

  IRValue* getResult() const { return Result; }
  Type* getAllocType() const { return AllocType; }

  std::string toString() const override;
};

/// Return: ret [value]
class IRRetInst : public IRInstruction {
  IRValue* RetValue;  // nullptr for void return

public:
  IRRetInst(IRValue* Val = nullptr)
      : IRInstruction(Ret), RetValue(Val) {}

  IRValue* getRetValue() const { return RetValue; }
  bool hasRetValue() const { return RetValue != nullptr; }

  // Setter for operand replacement
  void setRetValue(IRValue* V) { RetValue = V; }

  std::string toString() const override;
};

/// Unconditional branch: br label
class IRBrInst : public IRInstruction {
  IRValue* Target;

public:
  IRBrInst(IRValue* Target)
      : IRInstruction(Br), Target(Target) {}

  IRValue* getTarget() const { return Target; }

  std::string toString() const override;
};

/// Conditional branch: br cond, label_true, label_false
class IRCondBrInst : public IRInstruction {
  IRValue* Condition;
  IRValue* TrueLabel;
  IRValue* FalseLabel;

public:
  IRCondBrInst(IRValue* Cond, IRValue* TrueLabel, IRValue* FalseLabel)
      : IRInstruction(CondBr), Condition(Cond),
        TrueLabel(TrueLabel), FalseLabel(FalseLabel) {}

  IRValue* getCondition() const { return Condition; }
  IRValue* getTrueLabel() const { return TrueLabel; }
  IRValue* getFalseLabel() const { return FalseLabel; }

  // Setter for operand replacement
  void setCondition(IRValue* C) { Condition = C; }

  std::string toString() const override;
};

/// Call: result = call function(args)
class IRCallInst : public IRInstruction {
  IRValue* Result;  // nullptr for void functions
  std::string FuncName;
  std::vector<IRValue*> Args;

public:
  IRCallInst(IRValue* Result, std::string FuncName, std::vector<IRValue*> Args)
      : IRInstruction(Call), Result(Result),
        FuncName(std::move(FuncName)), Args(std::move(Args)) {}

  IRValue* getResult() const { return Result; }
  const std::string& getFuncName() const { return FuncName; }
  const std::vector<IRValue*>& getArgs() const { return Args; }

  std::string toString() const override;
};

/// Label: label:
class IRLabelInst : public IRInstruction {
  IRValue* Label;

public:
  IRLabelInst(IRValue* Label)
      : IRInstruction(IRInstruction::Label), Label(Label) {}

  IRValue* getLabel() const { return Label; }

  std::string toString() const override;
};

/// Move: result = operand
class IRMoveInst : public IRInstruction {
  IRValue* Result;
  IRValue* Operand;

public:
  IRMoveInst(IRValue* Result, IRValue* Operand)
      : IRInstruction(Move), Result(Result), Operand(Operand) {}

  IRValue* getResult() const { return Result; }
  IRValue* getOperand() const { return Operand; }

  std::string toString() const override;
};

/// Phi: result = phi [val1, block1], [val2, block2], ...
class IRPhiInst : public IRInstruction {
public:
  struct PhiEntry {
    IRValue* Value;
    IRBasicBlock* Block;
  };

private:
  IRValue* Result;
  std::vector<PhiEntry> Incomings;

public:
  IRPhiInst(IRValue* Result)
      : IRInstruction(Phi), Result(Result) {}

  IRValue* getResult() const { return Result; }

  void addIncoming(IRValue* Val, IRBasicBlock* BB) {
    Incomings.push_back({Val, BB});
  }

  void replaceIncomingValue(IRValue* Old, IRValue* New) {
    for (auto& Entry : Incomings) {
      if (Entry.Value == Old) {
        Entry.Value = New;
      }
    }
  }

  const std::vector<PhiEntry>& getIncomings() const { return Incomings; }
  size_t getNumIncomings() const { return Incomings.size(); }

  std::string toString() const override;
};

/// IRBasicBlock - sequence of instructions with CFG edges
class IRBasicBlock {
  std::string Name;
  std::vector<std::unique_ptr<IRInstruction>> Instructions;
  IRFunction* Parent = nullptr;

  // CFG edges
  std::vector<IRBasicBlock*> Predecessors;
  std::vector<IRBasicBlock*> Successors;

public:
  IRBasicBlock(std::string Name) : Name(std::move(Name)) {}

  const std::string& getName() const { return Name; }

  // Instruction management
  void addInstruction(std::unique_ptr<IRInstruction> Inst) {
    Inst->setParent(this);
    Instructions.push_back(std::move(Inst));
  }

  const std::vector<std::unique_ptr<IRInstruction>>& getInstructions() const {
    return Instructions;
  }

  std::vector<std::unique_ptr<IRInstruction>>& getInstructions() {
    return Instructions;
  }

  /// Remove an instruction from this block (returns ownership)
  std::unique_ptr<IRInstruction> removeInstruction(IRInstruction* I) {
    for (auto It = Instructions.begin(); It != Instructions.end(); ++It) {
      if (It->get() == I) {
        auto Inst = std::move(*It);
        Instructions.erase(It);
        Inst->setParent(nullptr);
        return Inst;
      }
    }
    return nullptr;
  }

  /// Insert instruction before terminator
  void insertBeforeTerminator(std::unique_ptr<IRInstruction> Inst) {
    Inst->setParent(this);
    if (Instructions.empty() || !Instructions.back()->isTerminator()) {
      // No terminator, just add at end
      Instructions.push_back(std::move(Inst));
    } else {
      // Insert before terminator
      Instructions.insert(Instructions.end() - 1, std::move(Inst));
    }
  }

  IRInstruction* getTerminator() const {
    if (Instructions.empty()) return nullptr;
    IRInstruction* Last = Instructions.back().get();
    return Last->isTerminator() ? Last : nullptr;
  }

  // Parent function
  IRFunction* getParent() const { return Parent; }
  void setParent(IRFunction* F) { Parent = F; }

  // CFG edge management
  void addSuccessor(IRBasicBlock* Succ) {
    Successors.push_back(Succ);
    Succ->Predecessors.push_back(this);
  }

  void removeSuccessor(IRBasicBlock* Succ) {
    auto it = std::find(Successors.begin(), Successors.end(), Succ);
    if (it != Successors.end()) {
      Successors.erase(it);
      auto pred_it = std::find(Succ->Predecessors.begin(),
                               Succ->Predecessors.end(), this);
      if (pred_it != Succ->Predecessors.end()) {
        Succ->Predecessors.erase(pred_it);
      }
    }
  }

  const std::vector<IRBasicBlock*>& getPredecessors() const { return Predecessors; }
  const std::vector<IRBasicBlock*>& getSuccessors() const { return Successors; }

  size_t getNumPredecessors() const { return Predecessors.size(); }
  size_t getNumSuccessors() const { return Successors.size(); }

  void print() const;
};

/// IRFunction - function with basic blocks
class IRFunction {
  std::string Name;
  Type* ReturnType;
  std::vector<IRValue*> Parameters;
  std::vector<std::unique_ptr<IRBasicBlock>> Blocks;
  std::vector<std::unique_ptr<IRValue>> Values;  // Owned values

public:
  IRFunction(std::string Name, Type* RetType)
      : Name(std::move(Name)), ReturnType(RetType) {}

  const std::string& getName() const { return Name; }
  Type* getReturnType() const { return ReturnType; }

  void addParameter(IRValue* Param) { Parameters.push_back(Param); }
  const std::vector<IRValue*>& getParameters() const { return Parameters; }

  IRBasicBlock* createBlock(const std::string& Name) {
    auto Block = std::make_unique<IRBasicBlock>(Name);
    IRBasicBlock* Ptr = Block.get();
    Ptr->setParent(this);
    Blocks.push_back(std::move(Block));
    return Ptr;
  }

  const std::vector<std::unique_ptr<IRBasicBlock>>& getBlocks() const {
    return Blocks;
  }

  IRValue* createValue(IRValue::ValueKind K, const std::string& Name, Type* Ty) {
    auto Val = std::make_unique<IRValue>(K, Name, Ty);
    IRValue* Ptr = Val.get();
    Values.push_back(std::move(Val));
    return Ptr;
  }

  IRValue* createConstant(int64_t Val) {
    auto Const = std::make_unique<IRValue>(Val);
    IRValue* Ptr = Const.get();
    Values.push_back(std::move(Const));
    return Ptr;
  }

  void print() const;
};

/// IRModule - collection of functions
class IRModule {
  std::vector<std::unique_ptr<IRFunction>> Functions;
  std::vector<std::unique_ptr<IRValue>> GlobalValues;

public:
  IRFunction* createFunction(const std::string& Name, Type* RetType) {
    auto Func = std::make_unique<IRFunction>(Name, RetType);
    IRFunction* Ptr = Func.get();
    Functions.push_back(std::move(Func));
    return Ptr;
  }

  IRValue* createGlobal(const std::string& Name, Type* Ty) {
    auto Val = std::make_unique<IRValue>(IRValue::VK_Global, Name, Ty);
    IRValue* Ptr = Val.get();
    GlobalValues.push_back(std::move(Val));
    return Ptr;
  }

  const std::vector<std::unique_ptr<IRFunction>>& getFunctions() const {
    return Functions;
  }

  void print() const;
};

} // namespace yac

#endif // YAC_CODEGEN_IR_H
