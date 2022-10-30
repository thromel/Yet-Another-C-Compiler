#include "yac/AST/AST.h"
#include "yac/Type/Type.h"
#include <gtest/gtest.h>

using namespace yac;

class ASTTest : public ::testing::Test {
protected:
  TypeContext TyCtx;
  SourceLocation Loc{1, 1, "test.c"};
  SourceRange Range{Loc};
};

TEST_F(ASTTest, IntegerLiteral) {
  IntegerLiteral IntLit(Range, 42);
  EXPECT_EQ(IntLit.getValue(), 42);
  EXPECT_EQ(IntLit.getKind(), ASTNode::NK_IntegerLiteral);
}

TEST_F(ASTTest, FloatLiteral) {
  FloatLiteral FloatLit(Range, 3.14);
  EXPECT_DOUBLE_EQ(FloatLit.getValue(), 3.14);
  EXPECT_EQ(FloatLit.getKind(), ASTNode::NK_FloatLiteral);
}

TEST_F(ASTTest, StringLiteral) {
  StringLiteral StrLit(Range, "hello");
  EXPECT_EQ(StrLit.getValue(), "hello");
  EXPECT_EQ(StrLit.getKind(), ASTNode::NK_StringLiteral);
}

TEST_F(ASTTest, BinaryOperator) {
  auto LHS = std::make_unique<IntegerLiteral>(Range, 1);
  auto RHS = std::make_unique<IntegerLiteral>(Range, 2);

  BinaryOperator BinOp(Range, BinaryOperatorKind::Add,
                       std::move(LHS), std::move(RHS));

  EXPECT_EQ(BinOp.getOp(), BinaryOperatorKind::Add);
  EXPECT_EQ(BinOp.getOpName(), std::string("+"));
  ASSERT_NE(BinOp.getLHS(), nullptr);
  ASSERT_NE(BinOp.getRHS(), nullptr);
}

TEST_F(ASTTest, UnaryOperator) {
  auto SubExpr = std::make_unique<IntegerLiteral>(Range, 42);

  UnaryOperator UnOp(Range, UnaryOperatorKind::Minus, std::move(SubExpr));

  EXPECT_EQ(UnOp.getOp(), UnaryOperatorKind::Minus);
  EXPECT_EQ(UnOp.getOpName(), std::string("-"));
  ASSERT_NE(UnOp.getSubExpr(), nullptr);
}

TEST_F(ASTTest, VarDecl) {
  auto* IntTy = TyCtx.getIntType();
  VarDecl Var(Range, "x", IntTy);

  EXPECT_EQ(Var.getName(), "x");
  EXPECT_EQ(Var.getType(), IntTy);
  EXPECT_FALSE(Var.hasInit());
  EXPECT_FALSE(Var.isArray());
}

TEST_F(ASTTest, VarDeclWithInit) {
  auto* IntTy = TyCtx.getIntType();
  auto Init = std::make_unique<IntegerLiteral>(Range, 42);

  VarDecl Var(Range, "x", IntTy, std::move(Init));

  EXPECT_EQ(Var.getName(), "x");
  EXPECT_TRUE(Var.hasInit());
  ASSERT_NE(Var.getInit(), nullptr);
}

TEST_F(ASTTest, FunctionDecl) {
  auto* IntTy = TyCtx.getIntType();
  std::vector<ParmVarDecl*> Params;

  FunctionDecl Func(Range, "foo", IntTy, Params);

  EXPECT_EQ(Func.getName(), "foo");
  EXPECT_EQ(Func.getReturnType(), IntTy);
  EXPECT_EQ(Func.getNumParams(), 0);
  EXPECT_FALSE(Func.hasBody());
  EXPECT_FALSE(Func.isDefined());
}

TEST_F(ASTTest, CompoundStmt) {
  std::vector<std::unique_ptr<Stmt>> Stmts;
  Stmts.push_back(std::make_unique<BreakStmt>(Range));
  Stmts.push_back(std::make_unique<ContinueStmt>(Range));

  CompoundStmt Compound(Range, std::move(Stmts));

  EXPECT_EQ(Compound.size(), 2);
  ASSERT_NE(Compound[0], nullptr);
  ASSERT_NE(Compound[1], nullptr);
}
