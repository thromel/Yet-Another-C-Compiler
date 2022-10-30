#include "yac/CodeGen/IRBuilder.h"

namespace yac {

// ===----------------------------------------------------------------------===
// Main entry point
// ===----------------------------------------------------------------------===

std::unique_ptr<IRModule> IRBuilder::generateIR(TranslationUnit* TU) {
  // Visit all declarations
  for (size_t i = 0; i < TU->size(); ++i) {
    visit(TU->operator[](i));
  }

  return std::move(Module);
}

// ===----------------------------------------------------------------------===
// Helper methods
// ===----------------------------------------------------------------------===

IRValue* IRBuilder::createTemp(Type* Ty) {
  std::string Name = "t" + std::to_string(TempCounter++);
  return CurrentFunc->createValue(IRValue::VK_Temp, Name, Ty);
}

IRValue* IRBuilder::createLabel(const std::string& Prefix) {
  std::string Name = Prefix + std::to_string(LabelCounter++);
  return CurrentFunc->createValue(IRValue::VK_Label, Name, nullptr);
}

IRInstruction::Opcode IRBuilder::getIROpcode(BinaryOperatorKind Op) {
  switch (Op) {
  case BinaryOperatorKind::Add: return IRInstruction::Add;
  case BinaryOperatorKind::Sub: return IRInstruction::Sub;
  case BinaryOperatorKind::Mul: return IRInstruction::Mul;
  case BinaryOperatorKind::Div: return IRInstruction::Div;
  case BinaryOperatorKind::Mod: return IRInstruction::Mod;
  case BinaryOperatorKind::And: return IRInstruction::And;
  case BinaryOperatorKind::Or: return IRInstruction::Or;
  case BinaryOperatorKind::Xor: return IRInstruction::Xor;
  case BinaryOperatorKind::Shl: return IRInstruction::Shl;
  case BinaryOperatorKind::Shr: return IRInstruction::Shr;
  case BinaryOperatorKind::EQ: return IRInstruction::Eq;
  case BinaryOperatorKind::NE: return IRInstruction::Ne;
  case BinaryOperatorKind::LT: return IRInstruction::Lt;
  case BinaryOperatorKind::LE: return IRInstruction::Le;
  case BinaryOperatorKind::GT: return IRInstruction::Gt;
  case BinaryOperatorKind::GE: return IRInstruction::Ge;
  default: return IRInstruction::Add; // Shouldn't reach
  }
}

IRInstruction::Opcode IRBuilder::getIROpcode(UnaryOperatorKind Op) {
  switch (Op) {
  case UnaryOperatorKind::Not: return IRInstruction::Not;
  case UnaryOperatorKind::BitwiseNot: return IRInstruction::Not;
  default: return IRInstruction::Not;
  }
}

// ===----------------------------------------------------------------------===
// Declaration visitors
// ===----------------------------------------------------------------------===

void IRBuilder::visitFunctionDecl(FunctionDecl* D) {
  // Create function
  CurrentFunc = Module->createFunction(D->getName(), D->getReturnType());

  // Add parameters
  for (ParmVarDecl* Param : D->getParams()) {
    IRValue* ParamVal = CurrentFunc->createValue(
        IRValue::VK_Local, Param->getName(), Param->getType());
    CurrentFunc->addParameter(ParamVal);
  }

  // Create entry block
  CurrentBlock = CurrentFunc->createBlock("entry");

  // Allocate stack space for parameters
  for (size_t i = 0; i < D->getParams().size(); ++i) {
    ParmVarDecl* Param = D->getParam(i);
    IRValue* Slot = createTemp(Param->getType());
    emit<IRAllocaInst>(Slot, Param->getType());
    LocalVars[Param] = Slot;

    // Store parameter value to slot
    IRValue* ParamVal = CurrentFunc->getParameters()[i];
    emit<IRStoreInst>(ParamVal, Slot);
  }

  // Reset temp/label counters for this function
  TempCounter = 0;
  LabelCounter = 0;

  // Generate body
  if (CompoundStmt* Body = D->getBody()) {
    visitCompoundStmt(Body);
  }

  // Add implicit return for void functions
  if (D->getReturnType()->isVoidType()) {
    emit<IRRetInst>(nullptr);
  }

  // Cleanup
  LocalVars.clear();
  CurrentFunc = nullptr;
  CurrentBlock = nullptr;
}

void IRBuilder::visitVarDecl(VarDecl* D) {
  // Allocate stack slot
  IRValue* Slot = createTemp(D->getType());
  emit<IRAllocaInst>(Slot, D->getType());
  LocalVars[D] = Slot;

  // Initialize if there's an initializer
  if (Expr* Init = D->getInit()) {
    visit(Init);
    IRValue* InitVal = LastExprValue;
    emit<IRStoreInst>(InitVal, Slot);
  }
}

void IRBuilder::visitParmVarDecl(ParmVarDecl* D) {
  // Parameters are handled in visitFunctionDecl
}

// ===----------------------------------------------------------------------===
// Statement visitors
// ===----------------------------------------------------------------------===

void IRBuilder::visitCompoundStmt(CompoundStmt* S) {
  for (size_t i = 0; i < S->size(); ++i) {
    visit(S->operator[](i));
  }
}

void IRBuilder::visitDeclStmt(DeclStmt* S) {
  visit(S->getDecl());
}

void IRBuilder::visitExprStmt(ExprStmt* S) {
  visit(S->getExpr());
  // Result is discarded
}

void IRBuilder::visitReturnStmt(ReturnStmt* S) {
  if (Expr* RetVal = S->getRetValue()) {
    visit(RetVal);
    emit<IRRetInst>(LastExprValue);
  } else {
    emit<IRRetInst>(nullptr);
  }
}

void IRBuilder::visitIfStmt(IfStmt* S) {
  // Evaluate condition
  visit(S->getCondition());
  IRValue* Cond = LastExprValue;

  // Create labels
  IRValue* ThenLabel = createLabel("then");
  IRValue* ElseLabel = S->hasElse() ? createLabel("else") : createLabel("endif");
  IRValue* EndLabel = createLabel("endif");

  // Conditional branch
  if (S->hasElse()) {
    emit<IRCondBrInst>(Cond, ThenLabel, ElseLabel);
  } else {
    emit<IRCondBrInst>(Cond, ThenLabel, EndLabel);
  }

  // Then block
  CurrentBlock = CurrentFunc->createBlock(ThenLabel->getName());
  emit<IRLabelInst>(ThenLabel);
  visit(S->getThen());
  emit<IRBrInst>(EndLabel);

  // Else block (if present)
  if (S->hasElse()) {
    CurrentBlock = CurrentFunc->createBlock(ElseLabel->getName());
    emit<IRLabelInst>(ElseLabel);
    visit(S->getElse());
    emit<IRBrInst>(EndLabel);
  }

  // End block
  CurrentBlock = CurrentFunc->createBlock(EndLabel->getName());
  emit<IRLabelInst>(EndLabel);
}

void IRBuilder::visitWhileStmt(WhileStmt* S) {
  IRValue* CondLabel = createLabel("while_cond");
  IRValue* BodyLabel = createLabel("while_body");
  IRValue* EndLabel = createLabel("while_end");

  // Branch to condition
  emit<IRBrInst>(CondLabel);

  // Condition block
  CurrentBlock = CurrentFunc->createBlock(CondLabel->getName());
  emit<IRLabelInst>(CondLabel);
  visit(S->getCondition());
  IRValue* Cond = LastExprValue;
  emit<IRCondBrInst>(Cond, BodyLabel, EndLabel);

  // Body block
  CurrentBlock = CurrentFunc->createBlock(BodyLabel->getName());
  emit<IRLabelInst>(BodyLabel);
  visit(S->getBody());
  emit<IRBrInst>(CondLabel);

  // End block
  CurrentBlock = CurrentFunc->createBlock(EndLabel->getName());
  emit<IRLabelInst>(EndLabel);
}

void IRBuilder::visitForStmt(ForStmt* S) {
  // Init
  if (S->getInit()) {
    visit(S->getInit());
  }

  IRValue* CondLabel = createLabel("for_cond");
  IRValue* BodyLabel = createLabel("for_body");
  IRValue* IncLabel = createLabel("for_inc");
  IRValue* EndLabel = createLabel("for_end");

  // Branch to condition
  emit<IRBrInst>(CondLabel);

  // Condition block
  CurrentBlock = CurrentFunc->createBlock(CondLabel->getName());
  emit<IRLabelInst>(CondLabel);
  if (S->getCondition()) {
    visit(S->getCondition());
    IRValue* Cond = LastExprValue;
    emit<IRCondBrInst>(Cond, BodyLabel, EndLabel);
  } else {
    emit<IRBrInst>(BodyLabel);
  }

  // Body block
  CurrentBlock = CurrentFunc->createBlock(BodyLabel->getName());
  emit<IRLabelInst>(BodyLabel);
  visit(S->getBody());
  emit<IRBrInst>(IncLabel);

  // Increment block
  CurrentBlock = CurrentFunc->createBlock(IncLabel->getName());
  emit<IRLabelInst>(IncLabel);
  if (S->getIncrement()) {
    visit(S->getIncrement());
  }
  emit<IRBrInst>(CondLabel);

  // End block
  CurrentBlock = CurrentFunc->createBlock(EndLabel->getName());
  emit<IRLabelInst>(EndLabel);
}

void IRBuilder::visitDoStmt(DoStmt* S) {
  IRValue* BodyLabel = createLabel("do_body");
  IRValue* CondLabel = createLabel("do_cond");
  IRValue* EndLabel = createLabel("do_end");

  // Branch to body
  emit<IRBrInst>(BodyLabel);

  // Body block
  CurrentBlock = CurrentFunc->createBlock(BodyLabel->getName());
  emit<IRLabelInst>(BodyLabel);
  visit(S->getBody());
  emit<IRBrInst>(CondLabel);

  // Condition block
  CurrentBlock = CurrentFunc->createBlock(CondLabel->getName());
  emit<IRLabelInst>(CondLabel);
  visit(S->getCondition());
  IRValue* Cond = LastExprValue;
  emit<IRCondBrInst>(Cond, BodyLabel, EndLabel);

  // End block
  CurrentBlock = CurrentFunc->createBlock(EndLabel->getName());
  emit<IRLabelInst>(EndLabel);
}

void IRBuilder::visitBreakStmt(BreakStmt* S) {
  // TODO: Need to track loop end labels for break/continue
  // For now, emit a placeholder
}

void IRBuilder::visitContinueStmt(ContinueStmt* S) {
  // TODO: Need to track loop condition labels for break/continue
  // For now, emit a placeholder
}

// ===----------------------------------------------------------------------===
// Expression visitors
// ===----------------------------------------------------------------------===

void IRBuilder::visitIntegerLiteral(IntegerLiteral* E) {
  LastExprValue = CurrentFunc->createConstant(E->getValue());
}

void IRBuilder::visitFloatLiteral(FloatLiteral* E) {
  // For simplicity, treat float as int for now
  // A real implementation would handle float constants properly
  LastExprValue = CurrentFunc->createConstant((int64_t)E->getValue());
}

void IRBuilder::visitCharLiteral(CharLiteral* E) {
  LastExprValue = CurrentFunc->createConstant((int64_t)E->getValue());
}

void IRBuilder::visitStringLiteral(StringLiteral* E) {
  // TODO: String literals need to be stored as global data
  // For now, create a placeholder
  LastExprValue = CurrentFunc->createConstant(0);
}

void IRBuilder::visitDeclRefExpr(DeclRefExpr* E) {
  VarDecl* Var = E->getDecl();
  if (!Var) {
    // Function reference for calls
    LastExprValue = nullptr;
    return;
  }

  // Load from variable's stack slot
  IRValue* Slot = LocalVars[Var];
  IRValue* Result = createTemp(Var->getType());
  emit<IRLoadInst>(Result, Slot);
  LastExprValue = Result;
}

void IRBuilder::visitBinaryOperator(BinaryOperator* E) {
  BinaryOperatorKind Op = E->getOp();

  // Handle assignment separately
  if (Op == BinaryOperatorKind::Assign ||
      Op == BinaryOperatorKind::AddAssign ||
      Op == BinaryOperatorKind::SubAssign ||
      Op == BinaryOperatorKind::MulAssign ||
      Op == BinaryOperatorKind::DivAssign) {
    emitAssignment(E);
    return;
  }

  // Handle logical operators with short-circuit evaluation
  if (Op == BinaryOperatorKind::LAnd || Op == BinaryOperatorKind::LOr) {
    IRValue* Result = createTemp(TyCtx.getIntType());
    IRValue* RHSLabel = createLabel("logical_rhs");
    IRValue* EndLabel = createLabel("logical_end");

    // Evaluate LHS
    visit(E->getLHS());
    IRValue* LHS = LastExprValue;

    if (Op == BinaryOperatorKind::LAnd) {
      // AND: if LHS is false, result is 0, else evaluate RHS
      IRValue* Zero = CurrentFunc->createConstant(0);
      emit<IRMoveInst>(Result, Zero);
      emit<IRCondBrInst>(LHS, RHSLabel, EndLabel);
    } else {
      // OR: if LHS is true, result is 1, else evaluate RHS
      IRValue* One = CurrentFunc->createConstant(1);
      emit<IRMoveInst>(Result, One);
      IRValue* RHSLabelVal = RHSLabel;
      IRValue* EndLabelVal = EndLabel;
      // Create temp for inverted condition
      IRValue* NotLHS = createTemp(TyCtx.getIntType());
      emit<IRUnaryInst>(IRInstruction::Not, NotLHS, LHS);
      emit<IRCondBrInst>(NotLHS, RHSLabelVal, EndLabelVal);
    }

    // RHS block
    CurrentBlock = CurrentFunc->createBlock(RHSLabel->getName());
    emit<IRLabelInst>(RHSLabel);
    visit(E->getRHS());
    IRValue* RHS = LastExprValue;
    emit<IRMoveInst>(Result, RHS);
    emit<IRBrInst>(EndLabel);

    // End block
    CurrentBlock = CurrentFunc->createBlock(EndLabel->getName());
    emit<IRLabelInst>(EndLabel);

    LastExprValue = Result;
    return;
  }

  // Regular binary operation
  emitBinaryOp(E);
}

void IRBuilder::emitBinaryOp(BinaryOperator* E) {
  // Evaluate operands
  visit(E->getLHS());
  IRValue* LHS = LastExprValue;

  visit(E->getRHS());
  IRValue* RHS = LastExprValue;

  // Determine result type (use int for now)
  Type* ResultType = TyCtx.getIntType();

  // Create result temporary
  IRValue* Result = createTemp(ResultType);

  // Emit operation
  IRInstruction::Opcode IROp = getIROpcode(E->getOp());
  emit<IRBinaryInst>(IROp, Result, LHS, RHS);

  LastExprValue = Result;
}

void IRBuilder::emitAssignment(BinaryOperator* E) {
  // Get LHS address (must be a variable)
  DeclRefExpr* LHSRef = dynamic_cast<DeclRefExpr*>(E->getLHS());
  if (!LHSRef) {
    // For simplicity, only handle variable assignments
    return;
  }

  VarDecl* Var = LHSRef->getDecl();
  IRValue* Slot = LocalVars[Var];

  // Evaluate RHS
  if (E->getOp() == BinaryOperatorKind::Assign) {
    // Simple assignment
    visit(E->getRHS());
    IRValue* RHS = LastExprValue;
    emit<IRStoreInst>(RHS, Slot);
    LastExprValue = RHS;
  } else {
    // Compound assignment: load, operate, store
    IRValue* LHS = createTemp(Var->getType());
    emit<IRLoadInst>(LHS, Slot);

    visit(E->getRHS());
    IRValue* RHS = LastExprValue;

    IRValue* Result = createTemp(Var->getType());

    // Convert compound assignment to regular operation
    BinaryOperatorKind Op;
    switch (E->getOp()) {
    case BinaryOperatorKind::AddAssign: Op = BinaryOperatorKind::Add; break;
    case BinaryOperatorKind::SubAssign: Op = BinaryOperatorKind::Sub; break;
    case BinaryOperatorKind::MulAssign: Op = BinaryOperatorKind::Mul; break;
    case BinaryOperatorKind::DivAssign: Op = BinaryOperatorKind::Div; break;
    default: Op = BinaryOperatorKind::Add; break;
    }

    IRInstruction::Opcode IROp = getIROpcode(Op);
    emit<IRBinaryInst>(IROp, Result, LHS, RHS);
    emit<IRStoreInst>(Result, Slot);
    LastExprValue = Result;
  }
}

void IRBuilder::visitUnaryOperator(UnaryOperator* E) {
  UnaryOperatorKind Op = E->getOp();

  if (Op == UnaryOperatorKind::PreInc || Op == UnaryOperatorKind::PreDec ||
      Op == UnaryOperatorKind::PostInc || Op == UnaryOperatorKind::PostDec) {
    // Increment/decrement
    DeclRefExpr* Ref = dynamic_cast<DeclRefExpr*>(E->getSubExpr());
    if (!Ref) return;

    VarDecl* Var = Ref->getDecl();
    IRValue* Slot = LocalVars[Var];

    // Load current value
    IRValue* Current = createTemp(Var->getType());
    emit<IRLoadInst>(Current, Slot);

    // Create one constant
    IRValue* One = CurrentFunc->createConstant(1);

    // Compute new value
    IRValue* NewVal = createTemp(Var->getType());
    IRInstruction::Opcode IROp = (Op == UnaryOperatorKind::PreInc || Op == UnaryOperatorKind::PostInc)
                                     ? IRInstruction::Add
                                     : IRInstruction::Sub;
    emit<IRBinaryInst>(IROp, NewVal, Current, One);

    // Store new value
    emit<IRStoreInst>(NewVal, Slot);

    // Result is new value for pre-inc/dec, old value for post-inc/dec
    LastExprValue = (Op == UnaryOperatorKind::PreInc || Op == UnaryOperatorKind::PreDec)
                        ? NewVal
                        : Current;
    return;
  }

  // Other unary operations
  visit(E->getSubExpr());
  IRValue* Operand = LastExprValue;

  if (Op == UnaryOperatorKind::Plus) {
    // Unary plus is a no-op
    LastExprValue = Operand;
    return;
  }

  if (Op == UnaryOperatorKind::Minus) {
    // Unary minus: 0 - operand
    IRValue* Zero = CurrentFunc->createConstant(0);
    IRValue* Result = createTemp(TyCtx.getIntType());
    emit<IRBinaryInst>(IRInstruction::Sub, Result, Zero, Operand);
    LastExprValue = Result;
    return;
  }

  // Not/BitwiseNot
  IRValue* Result = createTemp(TyCtx.getIntType());
  IRInstruction::Opcode IROp = getIROpcode(Op);
  emit<IRUnaryInst>(IROp, Result, Operand);
  LastExprValue = Result;
}

void IRBuilder::visitCallExpr(CallExpr* E) {
  // Evaluate arguments
  std::vector<IRValue*> Args;
  for (size_t i = 0; i < E->getNumArgs(); ++i) {
    visit(E->getArg(i));
    Args.push_back(LastExprValue);
  }

  // Get function name
  DeclRefExpr* CalleeRef = dynamic_cast<DeclRefExpr*>(E->getCallee());
  if (!CalleeRef) {
    LastExprValue = nullptr;
    return;
  }

  std::string FuncName = CalleeRef->getName();

  // Create result temporary (if non-void)
  IRValue* Result = nullptr;
  // For simplicity, assume all functions return int
  Result = createTemp(TyCtx.getIntType());

  // Emit call
  emit<IRCallInst>(Result, FuncName, Args);

  LastExprValue = Result;
}

void IRBuilder::visitArraySubscriptExpr(ArraySubscriptExpr* E) {
  // TODO: Array subscripting needs proper implementation
  LastExprValue = CurrentFunc->createConstant(0);
}

} // namespace yac
