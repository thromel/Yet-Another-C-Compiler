#ifndef YAC_PARSE_LEXER_H
#define YAC_PARSE_LEXER_H

#include "yac/Basic/Diagnostic.h"
#include "yac/Parse/Token.h"
#include <string>
#include <vector>

namespace yac {

/// Simple hand-written lexer for C subset
class Lexer {
  const std::string& Source;
  const char* Filename;
  size_t Position = 0;
  size_t Line = 1;
  size_t Column = 1;
  DiagnosticEngine& Diag;

public:
  Lexer(const std::string& Source, const char* Filename, DiagnosticEngine& Diag)
      : Source(Source), Filename(Filename), Diag(Diag) {}

  std::vector<Token> tokenize();

private:
  // Character access
  char currentChar() const {
    return Position < Source.size() ? Source[Position] : '\0';
  }

  char peek(size_t Offset = 1) const {
    size_t Pos = Position + Offset;
    return Pos < Source.size() ? Source[Pos] : '\0';
  }

  void advance() {
    if (Position < Source.size()) {
      if (Source[Position] == '\n') {
        Line++;
        Column = 1;
      } else {
        Column++;
      }
      Position++;
    }
  }

  void skipWhitespace();
  void skipComment();
  bool skipBlockComment();

  SourceLocation currentLocation() const {
    return SourceLocation(Line, Column, Filename);
  }

  // Token creation
  Token makeToken(TokenKind Kind, std::string Text);
  Token lexIdentifierOrKeyword();
  Token lexNumber();
  Token lexCharLiteral();
  Token lexStringLiteral();
  Token lexOperator();

  // Helpers
  bool isAlpha(char C) const;
  bool isDigit(char C) const;
  bool isAlnum(char C) const;
  TokenKind identifyKeyword(const std::string& Text);
};

} // namespace yac

#endif // YAC_PARSE_LEXER_H
