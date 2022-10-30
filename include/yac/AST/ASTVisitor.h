#ifndef YAC_AST_ASTVISITOR_H
#define YAC_AST_ASTVISITOR_H

#include "yac/AST/AST.h"

namespace yac {

/// Base class for AST visitors using the visitor pattern
class ASTVisitor {
public:
  virtual ~ASTVisitor() = default;

  // Visit expressions
  virtual void visitIntegerLiteral(IntegerLiteral* E) {}
  virtual void visitFloatLiteral(FloatLiteral* E) {}
  virtual void visitCharLiteral(CharLiteral* E) {}
  virtual void visitStringLiteral(StringLiteral* E) {}
  virtual void visitDeclRefExpr(DeclRefExpr* E) {}
  virtual void visitBinaryOperator(BinaryOperator* E) {}
  virtual void visitUnaryOperator(UnaryOperator* E) {}
  virtual void visitCallExpr(CallExpr* E) {}
  virtual void visitArraySubscriptExpr(ArraySubscriptExpr* E) {}
  virtual void visitImplicitCastExpr(ImplicitCastExpr* E) {}

  // Visit statements
  virtual void visitCompoundStmt(CompoundStmt* S) {}
  virtual void visitDeclStmt(DeclStmt* S) {}
  virtual void visitExprStmt(ExprStmt* S) {}
  virtual void visitReturnStmt(ReturnStmt* S) {}
  virtual void visitIfStmt(IfStmt* S) {}
  virtual void visitWhileStmt(WhileStmt* S) {}
  virtual void visitForStmt(ForStmt* S) {}
  virtual void visitDoStmt(DoStmt* S) {}
  virtual void visitBreakStmt(BreakStmt* S) {}
  virtual void visitContinueStmt(ContinueStmt* S) {}

  // Visit declarations
  virtual void visitVarDecl(VarDecl* D) {}
  virtual void visitParmVarDecl(ParmVarDecl* D) {}
  virtual void visitFunctionDecl(FunctionDecl* D) {}
  virtual void visitTranslationUnit(TranslationUnit* TU) {}

  // Generic visit - dispatches to specific visit methods
  void visit(ASTNode* Node);
  void visit(Expr* E);
  void visit(Stmt* S);
  void visit(Decl* D);
};

/// AST pretty printer
class ASTPrinter : public ASTVisitor {
  std::ostream& OS;
  int Indent = 0;

  void printIndent();
  void increaseIndent() { Indent += 2; }
  void decreaseIndent() { Indent -= 2; }

public:
  ASTPrinter(std::ostream& OS) : OS(OS) {}

  // Expressions
  void visitIntegerLiteral(IntegerLiteral* E) override;
  void visitFloatLiteral(FloatLiteral* E) override;
  void visitCharLiteral(CharLiteral* E) override;
  void visitStringLiteral(StringLiteral* E) override;
  void visitDeclRefExpr(DeclRefExpr* E) override;
  void visitBinaryOperator(BinaryOperator* E) override;
  void visitUnaryOperator(UnaryOperator* E) override;
  void visitCallExpr(CallExpr* E) override;
  void visitArraySubscriptExpr(ArraySubscriptExpr* E) override;
  void visitImplicitCastExpr(ImplicitCastExpr* E) override;

  // Statements
  void visitCompoundStmt(CompoundStmt* S) override;
  void visitDeclStmt(DeclStmt* S) override;
  void visitExprStmt(ExprStmt* S) override;
  void visitReturnStmt(ReturnStmt* S) override;
  void visitIfStmt(IfStmt* S) override;
  void visitWhileStmt(WhileStmt* S) override;
  void visitForStmt(ForStmt* S) override;
  void visitDoStmt(DoStmt* S) override;
  void visitBreakStmt(BreakStmt* S) override;
  void visitContinueStmt(ContinueStmt* S) override;

  // Declarations
  void visitVarDecl(VarDecl* D) override;
  void visitParmVarDecl(ParmVarDecl* D) override;
  void visitFunctionDecl(FunctionDecl* D) override;
  void visitTranslationUnit(TranslationUnit* TU) override;
};

} // namespace yac

#endif // YAC_AST_ASTVISITOR_H
