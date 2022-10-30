#ifndef YAC_PARSE_TOKEN_H
#define YAC_PARSE_TOKEN_H

#include "yac/Basic/SourceLocation.h"
#include <string>

namespace yac {

/// Token kinds
enum class TokenKind {
  // End of file
  Eof,

  // Literals
  IntegerLiteral,
  FloatLiteral,
  CharLiteral,
  StringLiteral,

  // Identifiers
  Identifier,

  // Keywords
  KW_int,
  KW_float,
  KW_char,
  KW_void,
  KW_if,
  KW_else,
  KW_while,
  KW_for,
  KW_do,
  KW_return,
  KW_break,
  KW_continue,

  // Operators
  Plus,           // +
  Minus,          // -
  Star,           // *
  Slash,          // /
  Percent,        // %
  Equal,          // =
  PlusEqual,      // +=
  MinusEqual,     // -=
  StarEqual,      // *=
  SlashEqual,     // /=
  Less,           // <
  Greater,        // >
  LessEqual,      // <=
  GreaterEqual,   // >=
  EqualEqual,     // ==
  NotEqual,       // !=
  AmpAmp,         // &&
  PipePipe,       // ||
  Exclaim,        // !
  Amp,            // &
  Pipe,           // |
  Caret,          // ^
  Tilde,          // ~
  LessLess,       // <<
  GreaterGreater, // >>
  PlusPlus,       // ++
  MinusMinus,     // --

  // Punctuation
  LParen,         // (
  RParen,         // )
  LBrace,         // {
  RBrace,         // }
  LBracket,       // [
  RBracket,       // ]
  Comma,          // ,
  Semicolon,      // ;

  // Special
  Unknown
};

/// Token - represents a lexical token
class Token {
  TokenKind Kind;
  std::string Text;
  SourceLocation Loc;

  // For literal values
  union {
    int64_t IntValue;
    double FloatValue;
    char CharValue;
  };

public:
  Token() : Kind(TokenKind::Unknown), IntValue(0) {}

  Token(TokenKind Kind, std::string Text, SourceLocation Loc)
      : Kind(Kind), Text(std::move(Text)), Loc(Loc), IntValue(0) {}

  // Getters
  TokenKind getKind() const { return Kind; }
  const std::string& getText() const { return Text; }
  SourceLocation getLocation() const { return Loc; }

  // Type queries
  bool is(TokenKind K) const { return Kind == K; }
  bool isNot(TokenKind K) const { return Kind != K; }
  bool isOneOf(TokenKind K1, TokenKind K2) const {
    return is(K1) || is(K2);
  }
  template<typename... Ts>
  bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }

  // Literal values
  void setIntValue(int64_t Val) { IntValue = Val; }
  void setFloatValue(double Val) { FloatValue = Val; }
  void setCharValue(char Val) { CharValue = Val; }

  int64_t getIntValue() const { return IntValue; }
  double getFloatValue() const { return FloatValue; }
  char getCharValue() const { return CharValue; }

  // Helpers
  bool isLiteral() const {
    return isOneOf(TokenKind::IntegerLiteral, TokenKind::FloatLiteral,
                   TokenKind::CharLiteral, TokenKind::StringLiteral);
  }

  bool isKeyword() const {
    return Kind >= TokenKind::KW_int && Kind <= TokenKind::KW_continue;
  }

  bool isOperator() const {
    return Kind >= TokenKind::Plus && Kind <= TokenKind::MinusMinus;
  }

  // String representation
  static const char* getTokenKindName(TokenKind K);
  const char* getKindName() const { return getTokenKindName(Kind); }
};

} // namespace yac

#endif // YAC_PARSE_TOKEN_H
