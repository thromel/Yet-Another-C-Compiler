#ifndef YAC_TYPE_TYPE_H
#define YAC_TYPE_TYPE_H

#include <memory>
#include <string>
#include <vector>

namespace yac {

// Forward declarations
class FunctionDecl;

/// Base class for all types
class Type {
public:
  enum TypeKind {
    TK_Void,
    TK_Int,
    TK_Float,
    TK_Char,
    TK_Pointer,
    TK_Array,
    TK_Function
  };

private:
  TypeKind Kind;

protected:
  Type(TypeKind K) : Kind(K) {}

public:
  virtual ~Type() = default;

  TypeKind getKind() const { return Kind; }

  // Type queries
  bool isVoidType() const { return Kind == TK_Void; }
  bool isIntType() const { return Kind == TK_Int; }
  bool isFloatType() const { return Kind == TK_Float; }
  bool isCharType() const { return Kind == TK_Char; }
  bool isPointerType() const { return Kind == TK_Pointer; }
  bool isArrayType() const { return Kind == TK_Array; }
  bool isFunctionType() const { return Kind == TK_Function; }

  bool isArithmeticType() const {
    return isIntType() || isFloatType() || isCharType();
  }

  bool isScalarType() const {
    return isArithmeticType() || isPointerType();
  }

  // String representation
  virtual std::string toString() const = 0;

  // Type comparison
  virtual bool equals(const Type* Other) const = 0;

  // Type compatibility (considers implicit conversions)
  bool isCompatibleWith(const Type* Other) const;
};

/// Void type
class VoidType : public Type {
public:
  VoidType() : Type(TK_Void) {}

  std::string toString() const override { return "void"; }
  bool equals(const Type* Other) const override {
    return Other && Other->isVoidType();
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Void; }
};

/// Integer type
class IntType : public Type {
public:
  IntType() : Type(TK_Int) {}

  std::string toString() const override { return "int"; }
  bool equals(const Type* Other) const override {
    return Other && Other->isIntType();
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Int; }
};

/// Floating-point type
class FloatType : public Type {
public:
  FloatType() : Type(TK_Float) {}

  std::string toString() const override { return "float"; }
  bool equals(const Type* Other) const override {
    return Other && Other->isFloatType();
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Float; }
};

/// Character type
class CharType : public Type {
public:
  CharType() : Type(TK_Char) {}

  std::string toString() const override { return "char"; }
  bool equals(const Type* Other) const override {
    return Other && Other->isCharType();
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Char; }
};

/// Pointer type
class PointerType : public Type {
  Type* PointeeType;

public:
  PointerType(Type* Pointee) : Type(TK_Pointer), PointeeType(Pointee) {}

  Type* getPointeeType() const { return PointeeType; }

  std::string toString() const override {
    return PointeeType->toString() + "*";
  }

  bool equals(const Type* Other) const override {
    if (!Other || !Other->isPointerType())
      return false;
    return PointeeType->equals(
        static_cast<const PointerType*>(Other)->PointeeType);
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Pointer; }
};

/// Array type
class ArrayType : public Type {
  Type* ElementType;
  int Size; // -1 for unsized arrays

public:
  ArrayType(Type* Element, int Size = -1)
      : Type(TK_Array), ElementType(Element), Size(Size) {}

  Type* getElementType() const { return ElementType; }
  int getSize() const { return Size; }
  bool isSized() const { return Size >= 0; }

  std::string toString() const override {
    std::string result = ElementType->toString() + "[";
    if (isSized())
      result += std::to_string(Size);
    result += "]";
    return result;
  }

  bool equals(const Type* Other) const override {
    if (!Other || !Other->isArrayType())
      return false;
    auto* OtherArray = static_cast<const ArrayType*>(Other);
    return ElementType->equals(OtherArray->ElementType) &&
           Size == OtherArray->Size;
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Array; }
};

/// Function type
class FunctionType : public Type {
  Type* ReturnType;
  std::vector<Type*> ParamTypes;

public:
  FunctionType(Type* RetType, std::vector<Type*> Params)
      : Type(TK_Function), ReturnType(RetType),
        ParamTypes(std::move(Params)) {}

  Type* getReturnType() const { return ReturnType; }

  size_t getNumParams() const { return ParamTypes.size(); }
  Type* getParamType(size_t i) const { return ParamTypes[i]; }
  const std::vector<Type*>& getParamTypes() const { return ParamTypes; }

  std::string toString() const override {
    std::string result = ReturnType->toString() + "(";
    for (size_t i = 0; i < ParamTypes.size(); ++i) {
      if (i > 0) result += ", ";
      result += ParamTypes[i]->toString();
    }
    result += ")";
    return result;
  }

  bool equals(const Type* Other) const override {
    if (!Other || !Other->isFunctionType())
      return false;
    auto* OtherFunc = static_cast<const FunctionType*>(Other);
    if (!ReturnType->equals(OtherFunc->ReturnType))
      return false;
    if (ParamTypes.size() != OtherFunc->ParamTypes.size())
      return false;
    for (size_t i = 0; i < ParamTypes.size(); ++i) {
      if (!ParamTypes[i]->equals(OtherFunc->ParamTypes[i]))
        return false;
    }
    return true;
  }

  static bool classof(const Type* T) { return T->getKind() == TK_Function; }
};

/// Type context - manages type instances (singleton pattern)
class TypeContext {
  std::unique_ptr<VoidType> VoidTy;
  std::unique_ptr<IntType> IntTy;
  std::unique_ptr<FloatType> FloatTy;
  std::unique_ptr<CharType> CharTy;

  std::vector<std::unique_ptr<PointerType>> PointerTypes;
  std::vector<std::unique_ptr<ArrayType>> ArrayTypes;
  std::vector<std::unique_ptr<FunctionType>> FunctionTypes;

public:
  TypeContext();

  // Get basic types
  VoidType* getVoidType() { return VoidTy.get(); }
  IntType* getIntType() { return IntTy.get(); }
  FloatType* getFloatType() { return FloatTy.get(); }
  CharType* getCharType() { return CharTy.get(); }

  // Get or create complex types
  PointerType* getPointerType(Type* Pointee);
  ArrayType* getArrayType(Type* Element, int Size = -1);
  FunctionType* getFunctionType(Type* RetType, std::vector<Type*> Params);
};

} // namespace yac

#endif // YAC_TYPE_TYPE_H
