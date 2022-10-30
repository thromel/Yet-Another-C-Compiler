#include "yac/Sema/Sema.h"
#include <cassert>

namespace yac {

// ===----------------------------------------------------------------------===
// Main analysis entry point
// ===----------------------------------------------------------------------===

void Sema::analyze(TranslationUnit* TU) {
  visitTranslationUnit(TU);
}

Type* Sema::getExprType(Expr* E) {
  visit(E);
  return LastExprType;
}

// ===----------------------------------------------------------------------===
// Symbol table helpers
// ===----------------------------------------------------------------------===

bool Sema::declareSymbol(const std::string& Name, Type* Ty, Decl* D,
                         Symbol::SymbolKind Kind) {
  auto Sym = std::make_unique<Symbol>(Kind, Name, Ty, D);
  if (!SymTab.insert(std::move(Sym))) {
    Diag.error(D->getLocation(),
               "Redeclaration of '" + Name + "'");
    return false;
  }
  return true;
}

Symbol* Sema::lookupSymbol(const std::string& Name, SourceLocation Loc) {
  Symbol* Sym = SymTab.lookup(Name);
  if (!Sym) {
    Diag.error(Loc, "Use of undeclared identifier '" + Name + "'");
  }
  return Sym;
}

// ===----------------------------------------------------------------------===
// Type checking helpers
// ===----------------------------------------------------------------------===

bool Sema::isArithmeticType(Type* Ty) {
  return Ty->isArithmeticType();
}

bool Sema::isIntegerType(Type* Ty) {
  return Ty->isIntType() || Ty->isCharType();
}

bool Sema::isScalarType(Type* Ty) {
  return Ty->isScalarType();
}

bool Sema::checkAssignmentTypes(Type* LHS, Type* RHS, SourceLocation Loc) {
  if (LHS->isCompatibleWith(RHS)) {
    return true;
  }

  Diag.error(Loc, "Incompatible types in assignment: cannot convert '" +
                      RHS->toString() + "' to '" + LHS->toString() + "'");
  return false;
}

// ===----------------------------------------------------------------------===
// Declaration visitors
// ===----------------------------------------------------------------------===

void Sema::visitTranslationUnit(TranslationUnit* TU) {
  for (size_t i = 0; i < TU->size(); ++i) {
    visit(TU->operator[](i));
  }
}

void Sema::visitVarDecl(VarDecl* D) {
  // Declare the variable in current scope
  declareSymbol(D->getName(), D->getType(), D, Symbol::SK_Variable);

  // Type check initializer if present
  if (Expr* Init = D->getInit()) {
    Type* InitType = getExprType(Init);
    if (InitType && D->getType()) {
      checkAssignmentTypes(D->getType(), InitType, Init->getLocation());
    }
  }
}

void Sema::visitParmVarDecl(ParmVarDecl* D) {
  // Declare parameter in current scope
  declareSymbol(D->getName(), D->getType(), D, Symbol::SK_Parameter);
}

void Sema::visitFunctionDecl(FunctionDecl* D) {
  // Create function type for the symbol table
  std::vector<Type*> ParamTypes;
  for (ParmVarDecl* Param : D->getParams()) {
    ParamTypes.push_back(Param->getType());
  }
  Type* FuncType = TyCtx.getFunctionType(D->getReturnType(), ParamTypes);

  // Declare function in global scope
  declareSymbol(D->getName(), FuncType, D, Symbol::SK_Function);

  // Save previous function context
  FunctionDecl* PrevFunction = CurrentFunction;
  CurrentFunction = D;

  // Enter function scope
  SymTab.pushScope();

  // Declare parameters
  for (ParmVarDecl* Param : D->getParams()) {
    visitParmVarDecl(Param);
  }

  // Check function body
  if (CompoundStmt* Body = D->getBody()) {
    visit(Body);

    // Check that non-void functions have a return statement
    // (This is a simplified check - proper checking requires control flow analysis)
    if (!D->getReturnType()->isVoidType()) {
      // Could add warning here if we don't see a return
    }
  }

  // Exit function scope
  SymTab.popScope();

  CurrentFunction = PrevFunction;
}

// ===----------------------------------------------------------------------===
// Statement visitors
// ===----------------------------------------------------------------------===

void Sema::visitCompoundStmt(CompoundStmt* S) {
  // Enter new scope for compound statement
  SymTab.pushScope();

  for (size_t i = 0; i < S->size(); ++i) {
    visit(S->operator[](i));
  }

  SymTab.popScope();
}

void Sema::visitDeclStmt(DeclStmt* S) {
  visit(S->getDecl());
}

void Sema::visitExprStmt(ExprStmt* S) {
  getExprType(S->getExpr());
}

void Sema::visitReturnStmt(ReturnStmt* S) {
  if (!CurrentFunction) {
    Diag.error(S->getLocation(),
               "Return statement outside of function");
    return;
  }

  Type* RetType = CurrentFunction->getReturnType();

  if (Expr* RetValue = S->getRetValue()) {
    Type* RetExprType = getExprType(RetValue);

    if (RetType->isVoidType()) {
      Diag.error(S->getLocation(),
                 "Void function should not return a value");
    } else if (RetExprType) {
      checkAssignmentTypes(RetType, RetExprType,
                          RetValue->getLocation());
    }
  } else {
    if (!RetType->isVoidType()) {
      Diag.error(S->getLocation(),
                 "Non-void function must return a value");
    }
  }
}

void Sema::visitIfStmt(IfStmt* S) {
  // Check condition
  Type* CondType = getExprType(S->getCondition());
  if (CondType && !isScalarType(CondType)) {
    Diag.error(S->getCondition()->getLocation(),
               "Condition must have scalar type");
  }

  // Check branches
  visit(S->getThen());
  if (S->hasElse()) {
    visit(S->getElse());
  }
}

void Sema::visitWhileStmt(WhileStmt* S) {
  // Check condition
  Type* CondType = getExprType(S->getCondition());
  if (CondType && !isScalarType(CondType)) {
    Diag.error(S->getCondition()->getLocation(),
               "Loop condition must have scalar type");
  }

  // Check body with loop depth tracking
  LoopDepth++;
  visit(S->getBody());
  LoopDepth--;
}

void Sema::visitForStmt(ForStmt* S) {
  // Enter scope for loop (in case init declares variables)
  SymTab.pushScope();

  // Check init
  if (S->getInit()) {
    visit(S->getInit());
  }

  // Check condition
  if (S->getCondition()) {
    Type* CondType = getExprType(S->getCondition());
    if (CondType && !isScalarType(CondType)) {
      Diag.error(S->getCondition()->getLocation(),
                 "Loop condition must have scalar type");
    }
  }

  // Check increment
  if (S->getIncrement()) {
    getExprType(S->getIncrement());
  }

  // Check body with loop depth tracking
  LoopDepth++;
  visit(S->getBody());
  LoopDepth--;

  SymTab.popScope();
}

void Sema::visitDoStmt(DoStmt* S) {
  // Check body with loop depth tracking
  LoopDepth++;
  visit(S->getBody());
  LoopDepth--;

  // Check condition
  Type* CondType = getExprType(S->getCondition());
  if (CondType && !isScalarType(CondType)) {
    Diag.error(S->getCondition()->getLocation(),
               "Loop condition must have scalar type");
  }
}

void Sema::visitBreakStmt(BreakStmt* S) {
  if (LoopDepth == 0) {
    Diag.error(S->getLocation(),
               "Break statement not in loop");
  }
}

void Sema::visitContinueStmt(ContinueStmt* S) {
  if (LoopDepth == 0) {
    Diag.error(S->getLocation(),
               "Continue statement not in loop");
  }
}

// ===----------------------------------------------------------------------===
// Expression visitors - Literals
// ===----------------------------------------------------------------------===

void Sema::visitIntegerLiteral(IntegerLiteral* E) {
  LastExprType = TyCtx.getIntType();
}

void Sema::visitFloatLiteral(FloatLiteral* E) {
  LastExprType = TyCtx.getFloatType();
}

void Sema::visitCharLiteral(CharLiteral* E) {
  LastExprType = TyCtx.getCharType();
}

void Sema::visitStringLiteral(StringLiteral* E) {
  // String literal has type char*
  LastExprType = TyCtx.getPointerType(TyCtx.getCharType());
}

void Sema::visitDeclRefExpr(DeclRefExpr* E) {
  Symbol* Sym = lookupSymbol(E->getName(), E->getLocation());
  if (Sym) {
    // Only set decl if it's a VarDecl (variables or parameters)
    if (Sym->isVariable() || Sym->isParameter()) {
      if (VarDecl* VD = dynamic_cast<VarDecl*>(Sym->getDeclaration())) {
        E->setDecl(VD);
      }
    }
    LastExprType = Sym->getType();
  } else {
    LastExprType = nullptr;
  }
}

// ===----------------------------------------------------------------------===
// Expression visitors - Operators
// ===----------------------------------------------------------------------===

void Sema::visitBinaryOperator(BinaryOperator* E) {
  LastExprType = checkBinaryOp(E);
}

void Sema::visitUnaryOperator(UnaryOperator* E) {
  LastExprType = checkUnaryOp(E);
}

void Sema::visitCallExpr(CallExpr* E) {
  LastExprType = checkCallExpr(E);
}

void Sema::visitArraySubscriptExpr(ArraySubscriptExpr* E) {
  LastExprType = checkArraySubscript(E);
}

// ===----------------------------------------------------------------------===
// Binary operator type checking
// ===----------------------------------------------------------------------===

Type* Sema::checkBinaryOp(BinaryOperator* E) {
  Type* LHS = getExprType(E->getLHS());
  Type* RHS = getExprType(E->getRHS());

  if (!LHS || !RHS) {
    return nullptr;
  }

  BinaryOperatorKind Op = E->getOp();

  // Assignment operators
  if (Op == BinaryOperatorKind::Assign ||
      Op == BinaryOperatorKind::AddAssign ||
      Op == BinaryOperatorKind::SubAssign ||
      Op == BinaryOperatorKind::MulAssign ||
      Op == BinaryOperatorKind::DivAssign) {

    checkAssignmentTypes(LHS, RHS, E->getLocation());
    return LHS;
  }

  // Arithmetic operators: +, -, *, /, %
  if (Op == BinaryOperatorKind::Add || Op == BinaryOperatorKind::Sub ||
      Op == BinaryOperatorKind::Mul || Op == BinaryOperatorKind::Div) {

    if (!isArithmeticType(LHS) || !isArithmeticType(RHS)) {
      Diag.error(E->getLocation(),
                 std::string("Invalid operands to binary operator '") +
                     E->getOpName() + "'");
      return nullptr;
    }

    // Usual arithmetic conversions: if either is float, result is float
    if (LHS->isFloatType() || RHS->isFloatType()) {
      return TyCtx.getFloatType();
    }
    return TyCtx.getIntType();
  }

  // Modulo operator: %
  if (Op == BinaryOperatorKind::Mod) {
    if (!isIntegerType(LHS) || !isIntegerType(RHS)) {
      Diag.error(E->getLocation(),
                 "Modulo operator requires integer operands");
      return nullptr;
    }
    return TyCtx.getIntType();
  }

  // Relational operators: <, >, <=, >=
  if (Op == BinaryOperatorKind::LT || Op == BinaryOperatorKind::GT ||
      Op == BinaryOperatorKind::LE || Op == BinaryOperatorKind::GE) {

    if (!isArithmeticType(LHS) || !isArithmeticType(RHS)) {
      Diag.error(E->getLocation(),
                 "Invalid operands to relational operator");
      return nullptr;
    }
    return TyCtx.getIntType();  // Result is int (boolean)
  }

  // Equality operators: ==, !=
  if (Op == BinaryOperatorKind::EQ || Op == BinaryOperatorKind::NE) {
    if (!isScalarType(LHS) || !isScalarType(RHS)) {
      Diag.error(E->getLocation(),
                 "Invalid operands to equality operator");
      return nullptr;
    }
    return TyCtx.getIntType();  // Result is int (boolean)
  }

  // Logical operators: &&, ||
  if (Op == BinaryOperatorKind::LAnd || Op == BinaryOperatorKind::LOr) {
    if (!isScalarType(LHS) || !isScalarType(RHS)) {
      Diag.error(E->getLocation(),
                 "Invalid operands to logical operator");
      return nullptr;
    }
    return TyCtx.getIntType();  // Result is int (boolean)
  }

  // Bitwise operators: &, |, ^, <<, >>
  if (Op == BinaryOperatorKind::And || Op == BinaryOperatorKind::Or ||
      Op == BinaryOperatorKind::Xor || Op == BinaryOperatorKind::Shl ||
      Op == BinaryOperatorKind::Shr) {

    if (!isIntegerType(LHS) || !isIntegerType(RHS)) {
      Diag.error(E->getLocation(),
                 "Bitwise operator requires integer operands");
      return nullptr;
    }
    return TyCtx.getIntType();
  }

  Diag.error(E->getLocation(),
             "Unknown binary operator");
  return nullptr;
}

// ===----------------------------------------------------------------------===
// Unary operator type checking
// ===----------------------------------------------------------------------===

Type* Sema::checkUnaryOp(UnaryOperator* E) {
  Type* SubType = getExprType(E->getSubExpr());
  if (!SubType) {
    return nullptr;
  }

  UnaryOperatorKind Op = E->getOp();

  // Arithmetic unary: +, -
  if (Op == UnaryOperatorKind::Plus || Op == UnaryOperatorKind::Minus) {
    if (!isArithmeticType(SubType)) {
      Diag.error(E->getLocation(),
                 "Unary operator requires arithmetic operand");
      return nullptr;
    }
    return SubType;
  }

  // Logical not: !  (Note: Not is actually logical not in our enum)
  if (Op == UnaryOperatorKind::Not) {
    if (!isScalarType(SubType)) {
      Diag.error(E->getLocation(),
                 "Logical not requires scalar operand");
      return nullptr;
    }
    return TyCtx.getIntType();
  }

  // Bitwise not: ~
  if (Op == UnaryOperatorKind::BitwiseNot) {
    if (!isIntegerType(SubType)) {
      Diag.error(E->getLocation(),
                 "Bitwise not requires integer operand");
      return nullptr;
    }
    return TyCtx.getIntType();
  }

  // Address-of: &
  if (Op == UnaryOperatorKind::AddrOf) {
    return TyCtx.getPointerType(SubType);
  }

  // Dereference: *
  if (Op == UnaryOperatorKind::Deref) {
    if (!SubType->isPointerType()) {
      Diag.error(E->getLocation(),
                 "Dereference requires pointer operand");
      return nullptr;
    }
    PointerType* PT = static_cast<PointerType*>(SubType);
    return PT->getPointeeType();
  }

  // Pre/Post increment/decrement: ++, --
  if (Op == UnaryOperatorKind::PreInc || Op == UnaryOperatorKind::PreDec ||
      Op == UnaryOperatorKind::PostInc || Op == UnaryOperatorKind::PostDec) {

    if (!isScalarType(SubType)) {
      Diag.error(E->getLocation(),
                 "Increment/decrement requires scalar operand");
      return nullptr;
    }
    return SubType;
  }

  Diag.error(E->getLocation(), "Unknown unary operator");
  return nullptr;
}

// ===----------------------------------------------------------------------===
// Function call type checking
// ===----------------------------------------------------------------------===

Type* Sema::checkCallExpr(CallExpr* E) {
  // Get callee type
  Type* CalleeType = getExprType(E->getCallee());
  if (!CalleeType) {
    return nullptr;
  }

  // Callee must be a function type
  if (!CalleeType->isFunctionType()) {
    Diag.error(E->getCallee()->getLocation(),
               "Called object is not a function");
    return nullptr;
  }

  FunctionType* FT = static_cast<FunctionType*>(CalleeType);

  // Check argument count
  if (E->getNumArgs() != FT->getNumParams()) {
    Diag.error(E->getLocation(),
               "Function call has wrong number of arguments (expected " +
                   std::to_string(FT->getNumParams()) + ", got " +
                   std::to_string(E->getNumArgs()) + ")");
    return FT->getReturnType();
  }

  // Check argument types
  for (size_t i = 0; i < E->getNumArgs(); ++i) {
    Type* ArgType = getExprType(E->getArg(i));
    Type* ParamType = FT->getParamType(i);

    if (ArgType && ParamType) {
      checkAssignmentTypes(ParamType, ArgType,
                          E->getArg(i)->getLocation());
    }
  }

  return FT->getReturnType();
}

// ===----------------------------------------------------------------------===
// Array subscript type checking
// ===----------------------------------------------------------------------===

Type* Sema::checkArraySubscript(ArraySubscriptExpr* E) {
  Type* BaseType = getExprType(E->getBase());
  Type* IndexType = getExprType(E->getIndex());

  if (!BaseType || !IndexType) {
    return nullptr;
  }

  // Base must be array or pointer
  if (!BaseType->isArrayType() && !BaseType->isPointerType()) {
    Diag.error(E->getBase()->getLocation(),
               "Subscripted value is not an array or pointer");
    return nullptr;
  }

  // Index must be integer
  if (!isIntegerType(IndexType)) {
    Diag.error(E->getIndex()->getLocation(),
               "Array subscript must be an integer");
    return nullptr;
  }

  // Return element type
  if (BaseType->isArrayType()) {
    ArrayType* AT = static_cast<ArrayType*>(BaseType);
    return AT->getElementType();
  } else {
    PointerType* PT = static_cast<PointerType*>(BaseType);
    return PT->getPointeeType();
  }
}

} // namespace yac
