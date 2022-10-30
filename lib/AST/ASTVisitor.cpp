#include "yac/AST/ASTVisitor.h"
#include "yac/Type/Type.h"
#include <iostream>

namespace yac {

// Generic visit dispatcher
void ASTVisitor::visit(ASTNode* Node) {
  if (!Node) return;

  if (auto* E = dynamic_cast<Expr*>(Node)) {
    visit(E);
  } else if (auto* S = dynamic_cast<Stmt*>(Node)) {
    visit(S);
  } else if (auto* D = dynamic_cast<Decl*>(Node)) {
    visit(D);
  }
}

void ASTVisitor::visit(Expr* E) {
  if (!E) return;

  switch (E->getKind()) {
  case ASTNode::NK_IntegerLiteral:
    visitIntegerLiteral(static_cast<IntegerLiteral*>(E));
    break;
  case ASTNode::NK_FloatLiteral:
    visitFloatLiteral(static_cast<FloatLiteral*>(E));
    break;
  case ASTNode::NK_CharLiteral:
    visitCharLiteral(static_cast<CharLiteral*>(E));
    break;
  case ASTNode::NK_StringLiteral:
    visitStringLiteral(static_cast<StringLiteral*>(E));
    break;
  case ASTNode::NK_DeclRefExpr:
    visitDeclRefExpr(static_cast<DeclRefExpr*>(E));
    break;
  case ASTNode::NK_BinaryOperator:
    visitBinaryOperator(static_cast<BinaryOperator*>(E));
    break;
  case ASTNode::NK_UnaryOperator:
    visitUnaryOperator(static_cast<UnaryOperator*>(E));
    break;
  case ASTNode::NK_CallExpr:
    visitCallExpr(static_cast<CallExpr*>(E));
    break;
  case ASTNode::NK_ArraySubscript:
    visitArraySubscriptExpr(static_cast<ArraySubscriptExpr*>(E));
    break;
  case ASTNode::NK_ImplicitCastExpr:
    visitImplicitCastExpr(static_cast<ImplicitCastExpr*>(E));
    break;
  default:
    break;
  }
}

void ASTVisitor::visit(Stmt* S) {
  if (!S) return;

  switch (S->getKind()) {
  case ASTNode::NK_CompoundStmt:
    visitCompoundStmt(static_cast<CompoundStmt*>(S));
    break;
  case ASTNode::NK_DeclStmt:
    visitDeclStmt(static_cast<DeclStmt*>(S));
    break;
  case ASTNode::NK_ExprStmt:
    visitExprStmt(static_cast<ExprStmt*>(S));
    break;
  case ASTNode::NK_ReturnStmt:
    visitReturnStmt(static_cast<ReturnStmt*>(S));
    break;
  case ASTNode::NK_IfStmt:
    visitIfStmt(static_cast<IfStmt*>(S));
    break;
  case ASTNode::NK_WhileStmt:
    visitWhileStmt(static_cast<WhileStmt*>(S));
    break;
  case ASTNode::NK_ForStmt:
    visitForStmt(static_cast<ForStmt*>(S));
    break;
  case ASTNode::NK_DoStmt:
    visitDoStmt(static_cast<DoStmt*>(S));
    break;
  case ASTNode::NK_BreakStmt:
    visitBreakStmt(static_cast<BreakStmt*>(S));
    break;
  case ASTNode::NK_ContinueStmt:
    visitContinueStmt(static_cast<ContinueStmt*>(S));
    break;
  default:
    break;
  }
}

void ASTVisitor::visit(Decl* D) {
  if (!D) return;

  switch (D->getKind()) {
  case ASTNode::NK_VarDecl:
    visitVarDecl(static_cast<VarDecl*>(D));
    break;
  case ASTNode::NK_ParmVarDecl:
    visitParmVarDecl(static_cast<ParmVarDecl*>(D));
    break;
  case ASTNode::NK_FunctionDecl:
    visitFunctionDecl(static_cast<FunctionDecl*>(D));
    break;
  case ASTNode::NK_TranslationUnit:
    visitTranslationUnit(static_cast<TranslationUnit*>(D));
    break;
  default:
    break;
  }
}

// ===-------------------------------------------------------------------===
// ASTPrinter implementation
// ===-------------------------------------------------------------------===

void ASTPrinter::printIndent() {
  for (int i = 0; i < Indent; ++i) {
    OS << " ";
  }
}

// Expressions
void ASTPrinter::visitIntegerLiteral(IntegerLiteral* E) {
  printIndent();
  OS << "IntegerLiteral: " << E->getValue() << "\n";
}

void ASTPrinter::visitFloatLiteral(FloatLiteral* E) {
  printIndent();
  OS << "FloatLiteral: " << E->getValue() << "\n";
}

void ASTPrinter::visitCharLiteral(CharLiteral* E) {
  printIndent();
  OS << "CharLiteral: '" << E->getValue() << "'\n";
}

void ASTPrinter::visitStringLiteral(StringLiteral* E) {
  printIndent();
  OS << "StringLiteral: \"" << E->getValue() << "\"\n";
}

void ASTPrinter::visitDeclRefExpr(DeclRefExpr* E) {
  printIndent();
  OS << "DeclRefExpr: " << E->getName() << "\n";
}

void ASTPrinter::visitBinaryOperator(BinaryOperator* E) {
  printIndent();
  OS << "BinaryOperator: " << E->getOpName() << "\n";
  increaseIndent();
  visit(E->getLHS());
  visit(E->getRHS());
  decreaseIndent();
}

void ASTPrinter::visitUnaryOperator(UnaryOperator* E) {
  printIndent();
  OS << "UnaryOperator: " << E->getOpName() << "\n";
  increaseIndent();
  visit(E->getSubExpr());
  decreaseIndent();
}

void ASTPrinter::visitCallExpr(CallExpr* E) {
  printIndent();
  OS << "CallExpr:\n";
  increaseIndent();
  printIndent();
  OS << "Callee:\n";
  increaseIndent();
  visit(E->getCallee());
  decreaseIndent();
  if (E->getNumArgs() > 0) {
    printIndent();
    OS << "Args:\n";
    increaseIndent();
    for (size_t i = 0; i < E->getNumArgs(); ++i) {
      visit(E->getArg(i));
    }
    decreaseIndent();
  }
  decreaseIndent();
}

void ASTPrinter::visitArraySubscriptExpr(ArraySubscriptExpr* E) {
  printIndent();
  OS << "ArraySubscriptExpr:\n";
  increaseIndent();
  printIndent();
  OS << "Base:\n";
  increaseIndent();
  visit(E->getBase());
  decreaseIndent();
  printIndent();
  OS << "Index:\n";
  increaseIndent();
  visit(E->getIndex());
  decreaseIndent();
  decreaseIndent();
}

void ASTPrinter::visitImplicitCastExpr(ImplicitCastExpr* E) {
  printIndent();
  OS << "ImplicitCastExpr:\n";
  increaseIndent();
  visit(E->getSubExpr());
  decreaseIndent();
}

// Statements
void ASTPrinter::visitCompoundStmt(CompoundStmt* S) {
  printIndent();
  OS << "CompoundStmt:\n";
  increaseIndent();
  for (size_t i = 0; i < S->size(); ++i) {
    visit((*S)[i]);
  }
  decreaseIndent();
}

void ASTPrinter::visitDeclStmt(DeclStmt* S) {
  printIndent();
  OS << "DeclStmt:\n";
  increaseIndent();
  visit(S->getDecl());
  decreaseIndent();
}

void ASTPrinter::visitExprStmt(ExprStmt* S) {
  printIndent();
  OS << "ExprStmt:\n";
  increaseIndent();
  visit(S->getExpr());
  decreaseIndent();
}

void ASTPrinter::visitReturnStmt(ReturnStmt* S) {
  printIndent();
  OS << "ReturnStmt:\n";
  if (S->hasRetValue()) {
    increaseIndent();
    visit(S->getRetValue());
    decreaseIndent();
  }
}

void ASTPrinter::visitIfStmt(IfStmt* S) {
  printIndent();
  OS << "IfStmt:\n";
  increaseIndent();
  printIndent();
  OS << "Condition:\n";
  increaseIndent();
  visit(S->getCondition());
  decreaseIndent();
  printIndent();
  OS << "Then:\n";
  increaseIndent();
  visit(S->getThen());
  decreaseIndent();
  if (S->hasElse()) {
    printIndent();
    OS << "Else:\n";
    increaseIndent();
    visit(S->getElse());
    decreaseIndent();
  }
  decreaseIndent();
}

void ASTPrinter::visitWhileStmt(WhileStmt* S) {
  printIndent();
  OS << "WhileStmt:\n";
  increaseIndent();
  printIndent();
  OS << "Condition:\n";
  increaseIndent();
  visit(S->getCondition());
  decreaseIndent();
  printIndent();
  OS << "Body:\n";
  increaseIndent();
  visit(S->getBody());
  decreaseIndent();
  decreaseIndent();
}

void ASTPrinter::visitForStmt(ForStmt* S) {
  printIndent();
  OS << "ForStmt:\n";
  increaseIndent();
  if (S->getInit()) {
    printIndent();
    OS << "Init:\n";
    increaseIndent();
    visit(S->getInit());
    decreaseIndent();
  }
  if (S->getCondition()) {
    printIndent();
    OS << "Condition:\n";
    increaseIndent();
    visit(S->getCondition());
    decreaseIndent();
  }
  if (S->getIncrement()) {
    printIndent();
    OS << "Increment:\n";
    increaseIndent();
    visit(S->getIncrement());
    decreaseIndent();
  }
  printIndent();
  OS << "Body:\n";
  increaseIndent();
  visit(S->getBody());
  decreaseIndent();
  decreaseIndent();
}

void ASTPrinter::visitDoStmt(DoStmt* S) {
  printIndent();
  OS << "DoStmt:\n";
  increaseIndent();
  printIndent();
  OS << "Body:\n";
  increaseIndent();
  visit(S->getBody());
  decreaseIndent();
  printIndent();
  OS << "Condition:\n";
  increaseIndent();
  visit(S->getCondition());
  decreaseIndent();
  decreaseIndent();
}

void ASTPrinter::visitBreakStmt(BreakStmt* S) {
  printIndent();
  OS << "BreakStmt\n";
}

void ASTPrinter::visitContinueStmt(ContinueStmt* S) {
  printIndent();
  OS << "ContinueStmt\n";
}

// Declarations
void ASTPrinter::visitVarDecl(VarDecl* D) {
  printIndent();
  OS << "VarDecl: " << D->getName() << " : ";
  if (D->getType()) {
    OS << D->getType()->toString();
  }
  OS << "\n";
  if (D->hasInit()) {
    increaseIndent();
    printIndent();
    OS << "Init:\n";
    increaseIndent();
    visit(D->getInit());
    decreaseIndent();
    decreaseIndent();
  }
}

void ASTPrinter::visitParmVarDecl(ParmVarDecl* D) {
  printIndent();
  OS << "ParmVarDecl: " << D->getName() << " : ";
  if (D->getType()) {
    OS << D->getType()->toString();
  }
  OS << "\n";
}

void ASTPrinter::visitFunctionDecl(FunctionDecl* D) {
  printIndent();
  OS << "FunctionDecl: " << D->getName() << " : ";
  if (D->getReturnType()) {
    OS << D->getReturnType()->toString();
  }
  OS << "\n";
  increaseIndent();
  if (D->getNumParams() > 0) {
    printIndent();
    OS << "Params:\n";
    increaseIndent();
    for (size_t i = 0; i < D->getNumParams(); ++i) {
      visit(D->getParam(i));
    }
    decreaseIndent();
  }
  if (D->hasBody()) {
    printIndent();
    OS << "Body:\n";
    increaseIndent();
    visit(D->getBody());
    decreaseIndent();
  }
  decreaseIndent();
}

void ASTPrinter::visitTranslationUnit(TranslationUnit* TU) {
  OS << "TranslationUnit:\n";
  increaseIndent();
  for (size_t i = 0; i < TU->size(); ++i) {
    visit((*TU)[i]);
  }
  decreaseIndent();
}

} // namespace yac
