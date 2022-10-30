#include "yac/Type/Type.h"
#include <gtest/gtest.h>

using namespace yac;

class TypeTest : public ::testing::Test {
protected:
  TypeContext TyCtx;
};

TEST_F(TypeTest, BasicTypes) {
  auto* IntTy = TyCtx.getIntType();
  auto* FloatTy = TyCtx.getFloatType();
  auto* CharTy = TyCtx.getCharType();
  auto* VoidTy = TyCtx.getVoidType();

  EXPECT_TRUE(IntTy->isIntType());
  EXPECT_FALSE(IntTy->isFloatType());
  EXPECT_TRUE(FloatTy->isFloatType());
  EXPECT_TRUE(CharTy->isCharType());
  EXPECT_TRUE(VoidTy->isVoidType());
}

TEST_F(TypeTest, TypeToString) {
  EXPECT_EQ(TyCtx.getIntType()->toString(), "int");
  EXPECT_EQ(TyCtx.getFloatType()->toString(), "float");
  EXPECT_EQ(TyCtx.getCharType()->toString(), "char");
  EXPECT_EQ(TyCtx.getVoidType()->toString(), "void");
}

TEST_F(TypeTest, PointerTypes) {
  auto* IntTy = TyCtx.getIntType();
  auto* IntPtrTy = TyCtx.getPointerType(IntTy);

  EXPECT_TRUE(IntPtrTy->isPointerType());
  EXPECT_EQ(IntPtrTy->getPointeeType(), IntTy);
  EXPECT_EQ(IntPtrTy->toString(), "int*");

  // Pointer to pointer
  auto* IntPtrPtrTy = TyCtx.getPointerType(IntPtrTy);
  EXPECT_EQ(IntPtrPtrTy->toString(), "int**");
}

TEST_F(TypeTest, ArrayTypes) {
  auto* IntTy = TyCtx.getIntType();
  auto* ArrTy = TyCtx.getArrayType(IntTy, 10);

  EXPECT_TRUE(ArrTy->isArrayType());
  EXPECT_EQ(ArrTy->getElementType(), IntTy);
  EXPECT_EQ(ArrTy->getSize(), 10);
  EXPECT_EQ(ArrTy->toString(), "int[10]");
}

TEST_F(TypeTest, FunctionTypes) {
  auto* IntTy = TyCtx.getIntType();
  auto* FloatTy = TyCtx.getFloatType();

  std::vector<Type*> Params = {IntTy, FloatTy};
  auto* FuncTy = TyCtx.getFunctionType(IntTy, Params);

  EXPECT_TRUE(FuncTy->isFunctionType());
  EXPECT_EQ(FuncTy->getReturnType(), IntTy);
  EXPECT_EQ(FuncTy->getNumParams(), 2);
  EXPECT_EQ(FuncTy->getParamType(0), IntTy);
  EXPECT_EQ(FuncTy->getParamType(1), FloatTy);
}

TEST_F(TypeTest, TypeEquality) {
  auto* IntTy1 = TyCtx.getIntType();
  auto* IntTy2 = TyCtx.getIntType();
  auto* FloatTy = TyCtx.getFloatType();

  EXPECT_TRUE(IntTy1->equals(IntTy2));
  EXPECT_FALSE(IntTy1->equals(FloatTy));
}

TEST_F(TypeTest, TypeCompatibility) {
  auto* IntTy = TyCtx.getIntType();
  auto* FloatTy = TyCtx.getFloatType();
  auto* CharTy = TyCtx.getCharType();

  // Arithmetic types are compatible
  EXPECT_TRUE(IntTy->isCompatibleWith(FloatTy));
  EXPECT_TRUE(IntTy->isCompatibleWith(CharTy));
  EXPECT_TRUE(CharTy->isCompatibleWith(IntTy));
}
