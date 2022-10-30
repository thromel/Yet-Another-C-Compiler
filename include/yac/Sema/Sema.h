#ifndef YAC_SEMA_SEMA_H
#define YAC_SEMA_SEMA_H

#include "yac/AST/AST.h"
#include "yac/AST/ASTVisitor.h"
#include "yac/Basic/Diagnostic.h"
#include "yac/Sema/SymbolTable.h"
#include "yac/Type/Type.h"

namespace yac {

/// Sema - Semantic analyzer
/// Performs type checking, symbol resolution, and semantic validation
class Sema : public ASTVisitor {
  DiagnosticEngine& Diag;
  TypeContext& TyCtx;
  SymbolTable SymTab;

  // Context tracking
  FunctionDecl* CurrentFunction = nullptr;
  int LoopDepth = 0;  // Track if we're inside a loop (for break/continue)

  // Type of the last visited expression (for type checking)
  Type* LastExprType = nullptr;

public:
  Sema(DiagnosticEngine& Diag, TypeContext& TyCtx)
      : Diag(Diag), TyCtx(TyCtx) {}

  /// Analyze a translation unit
  void analyze(TranslationUnit* TU);

  /// Get the type of an expression (performs type checking as side effect)
  Type* getExprType(Expr* E);

  // Visitor methods for declarations
  void visitVarDecl(VarDecl* D) override;
  void visitParmVarDecl(ParmVarDecl* D) override;
  void visitFunctionDecl(FunctionDecl* D) override;
  void visitTranslationUnit(TranslationUnit* TU) override;

  // Visitor methods for statements
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

  // Visitor methods for expressions
  void visitIntegerLiteral(IntegerLiteral* E) override;
  void visitFloatLiteral(FloatLiteral* E) override;
  void visitCharLiteral(CharLiteral* E) override;
  void visitStringLiteral(StringLiteral* E) override;
  void visitDeclRefExpr(DeclRefExpr* E) override;
  void visitBinaryOperator(BinaryOperator* E) override;
  void visitUnaryOperator(UnaryOperator* E) override;
  void visitCallExpr(CallExpr* E) override;
  void visitArraySubscriptExpr(ArraySubscriptExpr* E) override;

private:
  /// Type checking helpers
  Type* checkBinaryOp(BinaryOperator* E);
  Type* checkUnaryOp(UnaryOperator* E);
  Type* checkCallExpr(CallExpr* E);
  Type* checkArraySubscript(ArraySubscriptExpr* E);

  /// Validation helpers
  bool checkAssignmentTypes(Type* LHS, Type* RHS, SourceLocation Loc);
  bool isArithmeticType(Type* Ty);
  bool isIntegerType(Type* Ty);
  bool isScalarType(Type* Ty);

  /// Symbol table helpers
  bool declareSymbol(const std::string& Name, Type* Ty, Decl* D, Symbol::SymbolKind Kind);
  Symbol* lookupSymbol(const std::string& Name, SourceLocation Loc);
};

} // namespace yac

#endif // YAC_SEMA_SEMA_H
