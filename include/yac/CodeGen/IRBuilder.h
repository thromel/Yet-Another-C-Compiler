#ifndef YAC_CODEGEN_IRBUILDER_H
#define YAC_CODEGEN_IRBUILDER_H

#include "yac/AST/AST.h"
#include "yac/AST/ASTVisitor.h"
#include "yac/CodeGen/IR.h"
#include "yac/Type/Type.h"
#include <map>
#include <memory>

namespace yac {

/// IRBuilder - converts AST to IR
class IRBuilder : public ASTVisitor {
  std::unique_ptr<IRModule> Module;
  TypeContext& TyCtx;

  // Current context
  IRFunction* CurrentFunc = nullptr;
  IRBasicBlock* CurrentBlock = nullptr;

  // Temporary management
  int TempCounter = 0;
  int LabelCounter = 0;

  // Symbol table: maps variables to their stack slots (alloca)
  std::map<VarDecl*, IRValue*> LocalVars;

  // Last computed expression value
  IRValue* LastExprValue = nullptr;

public:
  IRBuilder(TypeContext& TyCtx) : TyCtx(TyCtx) {
    Module = std::make_unique<IRModule>();
  }

  /// Generate IR from AST
  std::unique_ptr<IRModule> generateIR(TranslationUnit* TU);

  // Visitor methods
  void visitFunctionDecl(FunctionDecl* D) override;
  void visitVarDecl(VarDecl* D) override;
  void visitParmVarDecl(ParmVarDecl* D) override;

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
  // Helper methods
  IRValue* createTemp(Type* Ty);
  IRValue* createLabel(const std::string& Prefix);

  void emitBinaryOp(BinaryOperator* E);
  void emitUnaryOp(UnaryOperator* E);
  void emitAssignment(BinaryOperator* E);

  IRInstruction::Opcode getIROpcode(BinaryOperatorKind Op);
  IRInstruction::Opcode getIROpcode(UnaryOperatorKind Op);

  template<typename T, typename... Args>
  void emit(Args&&... args) {
    CurrentBlock->addInstruction(
        std::make_unique<T>(std::forward<Args>(args)...));
  }

  // Overload for directly emitting an already-constructed instruction
  void emit(std::unique_ptr<IRInstruction> Inst) {
    CurrentBlock->addInstruction(std::move(Inst));
  }
};

} // namespace yac

#endif // YAC_CODEGEN_IRBUILDER_H
