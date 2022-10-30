#include "yac/Type/Type.h"

namespace yac {

// Type compatibility check
bool Type::isCompatibleWith(const Type* Other) const {
  if (!Other) return false;
  if (equals(Other)) return true;

  // Int <-> Char (implicit conversion)
  if ((isIntType() && Other->isCharType()) ||
      (isCharType() && Other->isIntType())) {
    return true;
  }

  // Int <-> Float (implicit conversion)
  if ((isIntType() && Other->isFloatType()) ||
      (isFloatType() && Other->isIntType())) {
    return true;
  }

  // Char <-> Float (implicit conversion)
  if ((isCharType() && Other->isFloatType()) ||
      (isFloatType() && Other->isCharType())) {
    return true;
  }

  // Array to pointer decay
  if (isArrayType() && Other->isPointerType()) {
    auto* ArrTy = static_cast<const ArrayType*>(this);
    auto* PtrTy = static_cast<const PointerType*>(Other);
    return ArrTy->getElementType()->equals(PtrTy->getPointeeType());
  }

  return false;
}

// TypeContext implementation
TypeContext::TypeContext() {
  VoidTy = std::make_unique<VoidType>();
  IntTy = std::make_unique<IntType>();
  FloatTy = std::make_unique<FloatType>();
  CharTy = std::make_unique<CharType>();
}

PointerType* TypeContext::getPointerType(Type* Pointee) {
  // Check if we already have this type
  for (auto& PT : PointerTypes) {
    if (PT->getPointeeType()->equals(Pointee)) {
      return PT.get();
    }
  }
  // Create new pointer type
  PointerTypes.push_back(std::make_unique<PointerType>(Pointee));
  return PointerTypes.back().get();
}

ArrayType* TypeContext::getArrayType(Type* Element, int Size) {
  // Check if we already have this type
  for (auto& AT : ArrayTypes) {
    if (AT->getElementType()->equals(Element) && AT->getSize() == Size) {
      return AT.get();
    }
  }
  // Create new array type
  ArrayTypes.push_back(std::make_unique<ArrayType>(Element, Size));
  return ArrayTypes.back().get();
}

FunctionType* TypeContext::getFunctionType(Type* RetType,
                                            std::vector<Type*> Params) {
  // Check if we already have this type
  for (auto& FT : FunctionTypes) {
    if (FT->equals(RetType) &&
        FT->getNumParams() == Params.size()) {
      bool match = true;
      for (size_t i = 0; i < Params.size(); ++i) {
        if (!FT->getParamType(i)->equals(Params[i])) {
          match = false;
          break;
        }
      }
      if (match) return FT.get();
    }
  }
  // Create new function type
  FunctionTypes.push_back(
      std::make_unique<FunctionType>(RetType, std::move(Params)));
  return FunctionTypes.back().get();
}

} // namespace yac
