#include "yac/Parse/Token.h"

namespace yac {

const char* Token::getTokenKindName(TokenKind K) {
  switch (K) {
  case TokenKind::Eof: return "EOF";
  case TokenKind::IntegerLiteral: return "IntegerLiteral";
  case TokenKind::FloatLiteral: return "FloatLiteral";
  case TokenKind::CharLiteral: return "CharLiteral";
  case TokenKind::StringLiteral: return "StringLiteral";
  case TokenKind::Identifier: return "Identifier";
  case TokenKind::KW_int: return "int";
  case TokenKind::KW_float: return "float";
  case TokenKind::KW_char: return "char";
  case TokenKind::KW_void: return "void";
  case TokenKind::KW_if: return "if";
  case TokenKind::KW_else: return "else";
  case TokenKind::KW_while: return "while";
  case TokenKind::KW_for: return "for";
  case TokenKind::KW_do: return "do";
  case TokenKind::KW_return: return "return";
  case TokenKind::KW_break: return "break";
  case TokenKind::KW_continue: return "continue";
  case TokenKind::Plus: return "+";
  case TokenKind::Minus: return "-";
  case TokenKind::Star: return "*";
  case TokenKind::Slash: return "/";
  case TokenKind::Percent: return "%";
  case TokenKind::Equal: return "=";
  case TokenKind::PlusEqual: return "+=";
  case TokenKind::MinusEqual: return "-=";
  case TokenKind::StarEqual: return "*=";
  case TokenKind::SlashEqual: return "/=";
  case TokenKind::Less: return "<";
  case TokenKind::Greater: return ">";
  case TokenKind::LessEqual: return "<=";
  case TokenKind::GreaterEqual: return ">=";
  case TokenKind::EqualEqual: return "==";
  case TokenKind::NotEqual: return "!=";
  case TokenKind::AmpAmp: return "&&";
  case TokenKind::PipePipe: return "||";
  case TokenKind::Exclaim: return "!";
  case TokenKind::Amp: return "&";
  case TokenKind::Pipe: return "|";
  case TokenKind::Caret: return "^";
  case TokenKind::Tilde: return "~";
  case TokenKind::LessLess: return "<<";
  case TokenKind::GreaterGreater: return ">>";
  case TokenKind::PlusPlus: return "++";
  case TokenKind::MinusMinus: return "--";
  case TokenKind::LParen: return "(";
  case TokenKind::RParen: return ")";
  case TokenKind::LBrace: return "{";
  case TokenKind::RBrace: return "}";
  case TokenKind::LBracket: return "[";
  case TokenKind::RBracket: return "]";
  case TokenKind::Comma: return ",";
  case TokenKind::Semicolon: return ";";
  case TokenKind::Unknown: return "Unknown";
  }
  return "Unknown";
}

} // namespace yac
