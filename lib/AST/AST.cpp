#include "yac/AST/AST.h"

namespace yac {

// BinaryOperator implementation
const char* BinaryOperator::getOpName(BinaryOperatorKind Op) {
  switch (Op) {
  case BinaryOperatorKind::Add: return "+";
  case BinaryOperatorKind::Sub: return "-";
  case BinaryOperatorKind::Mul: return "*";
  case BinaryOperatorKind::Div: return "/";
  case BinaryOperatorKind::Mod: return "%";
  case BinaryOperatorKind::LT: return "<";
  case BinaryOperatorKind::GT: return ">";
  case BinaryOperatorKind::LE: return "<=";
  case BinaryOperatorKind::GE: return ">=";
  case BinaryOperatorKind::EQ: return "==";
  case BinaryOperatorKind::NE: return "!=";
  case BinaryOperatorKind::LAnd: return "&&";
  case BinaryOperatorKind::LOr: return "||";
  case BinaryOperatorKind::And: return "&";
  case BinaryOperatorKind::Or: return "|";
  case BinaryOperatorKind::Xor: return "^";
  case BinaryOperatorKind::Shl: return "<<";
  case BinaryOperatorKind::Shr: return ">>";
  case BinaryOperatorKind::Assign: return "=";
  case BinaryOperatorKind::AddAssign: return "+=";
  case BinaryOperatorKind::SubAssign: return "-=";
  case BinaryOperatorKind::MulAssign: return "*=";
  case BinaryOperatorKind::DivAssign: return "/=";
  case BinaryOperatorKind::ModAssign: return "%=";
  }
  return "<?>";
}

// UnaryOperator implementation
const char* UnaryOperator::getOpName(UnaryOperatorKind Op) {
  switch (Op) {
  case UnaryOperatorKind::Plus: return "+";
  case UnaryOperatorKind::Minus: return "-";
  case UnaryOperatorKind::Not: return "!";
  case UnaryOperatorKind::BitwiseNot: return "~";
  case UnaryOperatorKind::PreInc: return "++";
  case UnaryOperatorKind::PreDec: return "--";
  case UnaryOperatorKind::PostInc: return "++";
  case UnaryOperatorKind::PostDec: return "--";
  case UnaryOperatorKind::AddrOf: return "&";
  case UnaryOperatorKind::Deref: return "*";
  }
  return "<?>";
}

} // namespace yac
