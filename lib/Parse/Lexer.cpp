#include "yac/Parse/Lexer.h"
#include <cctype>

namespace yac {

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> Tokens;

  while (Position < Source.size()) {
    skipWhitespace();
    if (Position >= Source.size()) break;

    // Skip comments
    if (currentChar() == '/' && peek() == '/') {
      skipComment();
      continue;
    }
    if (currentChar() == '/' && peek() == '*') {
      if (!skipBlockComment()) {
        Diag.error(currentLocation(), "Unterminated block comment");
      }
      continue;
    }

    SourceLocation Loc = currentLocation();

    // Identifiers and keywords
    if (isAlpha(currentChar()) || currentChar() == '_') {
      Tokens.push_back(lexIdentifierOrKeyword());
      continue;
    }

    // Numbers
    if (isDigit(currentChar())) {
      Tokens.push_back(lexNumber());
      continue;
    }

    // Character literals
    if (currentChar() == '\'') {
      Tokens.push_back(lexCharLiteral());
      continue;
    }

    // String literals
    if (currentChar() == '"') {
      Tokens.push_back(lexStringLiteral());
      continue;
    }

    // Operators and punctuation
    Token Op = lexOperator();
    if (Op.getKind() != TokenKind::Unknown) {
      Tokens.push_back(Op);
      continue;
    }

    // Unknown character
    Diag.error(Loc, std::string("Unknown character: '") + currentChar() + "'");
    advance();
  }

  // Add EOF token
  Tokens.push_back(makeToken(TokenKind::Eof, ""));

  return Tokens;
}

void Lexer::skipWhitespace() {
  while (Position < Source.size() &&
         (currentChar() == ' ' || currentChar() == '\t' ||
          currentChar() == '\n' || currentChar() == '\r')) {
    advance();
  }
}

void Lexer::skipComment() {
  // Skip //
  advance();
  advance();

  // Skip until end of line
  while (Position < Source.size() && currentChar() != '\n') {
    advance();
  }
}

bool Lexer::skipBlockComment() {
  // Skip /*
  advance();
  advance();

  // Find */
  while (Position < Source.size()) {
    if (currentChar() == '*' && peek() == '/') {
      advance(); // *
      advance(); // /
      return true;
    }
    advance();
  }

  return false; // Unterminated
}

Token Lexer::makeToken(TokenKind Kind, std::string Text) {
  return Token(Kind, std::move(Text), currentLocation());
}

Token Lexer::lexIdentifierOrKeyword() {
  SourceLocation Loc = currentLocation();
  std::string Text;

  while (isAlnum(currentChar()) || currentChar() == '_') {
    Text += currentChar();
    advance();
  }

  TokenKind Kind = identifyKeyword(Text);
  Token Tok(Kind, Text, Loc);
  return Tok;
}

Token Lexer::lexNumber() {
  SourceLocation Loc = currentLocation();
  std::string Text;
  bool IsFloat = false;

  // Integer part
  while (isDigit(currentChar())) {
    Text += currentChar();
    advance();
  }

  // Fractional part
  if (currentChar() == '.' && isDigit(peek())) {
    IsFloat = true;
    Text += currentChar();
    advance();

    while (isDigit(currentChar())) {
      Text += currentChar();
      advance();
    }
  }

  // Exponent
  if (currentChar() == 'e' || currentChar() == 'E') {
    IsFloat = true;
    Text += currentChar();
    advance();

    if (currentChar() == '+' || currentChar() == '-') {
      Text += currentChar();
      advance();
    }

    while (isDigit(currentChar())) {
      Text += currentChar();
      advance();
    }
  }

  Token Tok(IsFloat ? TokenKind::FloatLiteral : TokenKind::IntegerLiteral,
            Text, Loc);

  if (IsFloat) {
    Tok.setFloatValue(std::stod(Text));
  } else {
    Tok.setIntValue(std::stoll(Text));
  }

  return Tok;
}

Token Lexer::lexCharLiteral() {
  SourceLocation Loc = currentLocation();
  std::string Text = "'";
  advance(); // Skip opening '

  char Value = 0;

  if (currentChar() == '\\') {
    advance();
    // Escape sequence
    switch (currentChar()) {
    case 'n': Value = '\n'; break;
    case 't': Value = '\t'; break;
    case 'r': Value = '\r'; break;
    case '0': Value = '\0'; break;
    case '\\': Value = '\\'; break;
    case '\'': Value = '\''; break;
    default:
      Diag.error(Loc, "Unknown escape sequence");
      Value = currentChar();
    }
    Text += '\\';
    Text += currentChar();
    advance();
  } else if (currentChar() != '\'' && currentChar() != '\0') {
    Value = currentChar();
    Text += currentChar();
    advance();
  }

  if (currentChar() == '\'') {
    Text += '\'';
    advance();
  } else {
    Diag.error(Loc, "Unterminated character literal");
  }

  Token Tok(TokenKind::CharLiteral, Text, Loc);
  Tok.setCharValue(Value);
  return Tok;
}

Token Lexer::lexStringLiteral() {
  SourceLocation Loc = currentLocation();
  std::string Text = "\"";
  std::string Value;
  advance(); // Skip opening "

  while (currentChar() != '"' && currentChar() != '\0') {
    if (currentChar() == '\\') {
      advance();
      // Escape sequence
      switch (currentChar()) {
      case 'n': Value += '\n'; Text += "\\n"; break;
      case 't': Value += '\t'; Text += "\\t"; break;
      case 'r': Value += '\r'; Text += "\\r"; break;
      case '0': Value += '\0'; Text += "\\0"; break;
      case '\\': Value += '\\'; Text += "\\\\"; break;
      case '"': Value += '"'; Text += "\\\""; break;
      default:
        Diag.error(Loc, "Unknown escape sequence");
        Value += currentChar();
        Text += currentChar();
      }
      advance();
    } else {
      Value += currentChar();
      Text += currentChar();
      advance();
    }
  }

  if (currentChar() == '"') {
    Text += '"';
    advance();
  } else {
    Diag.error(Loc, "Unterminated string literal");
  }

  return Token(TokenKind::StringLiteral, Value, Loc);
}

Token Lexer::lexOperator() {
  SourceLocation Loc = currentLocation();
  char C = currentChar();
  char Next = peek();

  // Two-character operators
  if (C == '+' && Next == '+') {
    advance(); advance();
    return makeToken(TokenKind::PlusPlus, "++");
  }
  if (C == '-' && Next == '-') {
    advance(); advance();
    return makeToken(TokenKind::MinusMinus, "--");
  }
  if (C == '+' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::PlusEqual, "+=");
  }
  if (C == '-' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::MinusEqual, "-=");
  }
  if (C == '*' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::StarEqual, "*=");
  }
  if (C == '/' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::SlashEqual, "/=");
  }
  if (C == '=' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::EqualEqual, "==");
  }
  if (C == '!' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::NotEqual, "!=");
  }
  if (C == '<' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::LessEqual, "<=");
  }
  if (C == '>' && Next == '=') {
    advance(); advance();
    return makeToken(TokenKind::GreaterEqual, ">=");
  }
  if (C == '<' && Next == '<') {
    advance(); advance();
    return makeToken(TokenKind::LessLess, "<<");
  }
  if (C == '>' && Next == '>') {
    advance(); advance();
    return makeToken(TokenKind::GreaterGreater, ">>");
  }
  if (C == '&' && Next == '&') {
    advance(); advance();
    return makeToken(TokenKind::AmpAmp, "&&");
  }
  if (C == '|' && Next == '|') {
    advance(); advance();
    return makeToken(TokenKind::PipePipe, "||");
  }

  // Single-character operators
  TokenKind Kind = TokenKind::Unknown;
  std::string Text(1, C);

  switch (C) {
  case '+': Kind = TokenKind::Plus; break;
  case '-': Kind = TokenKind::Minus; break;
  case '*': Kind = TokenKind::Star; break;
  case '/': Kind = TokenKind::Slash; break;
  case '%': Kind = TokenKind::Percent; break;
  case '=': Kind = TokenKind::Equal; break;
  case '<': Kind = TokenKind::Less; break;
  case '>': Kind = TokenKind::Greater; break;
  case '!': Kind = TokenKind::Exclaim; break;
  case '&': Kind = TokenKind::Amp; break;
  case '|': Kind = TokenKind::Pipe; break;
  case '^': Kind = TokenKind::Caret; break;
  case '~': Kind = TokenKind::Tilde; break;
  case '(': Kind = TokenKind::LParen; break;
  case ')': Kind = TokenKind::RParen; break;
  case '{': Kind = TokenKind::LBrace; break;
  case '}': Kind = TokenKind::RBrace; break;
  case '[': Kind = TokenKind::LBracket; break;
  case ']': Kind = TokenKind::RBracket; break;
  case ',': Kind = TokenKind::Comma; break;
  case ';': Kind = TokenKind::Semicolon; break;
  default: return Token(); // Unknown
  }

  advance();
  return Token(Kind, Text, Loc);
}

bool Lexer::isAlpha(char C) const {
  return (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z');
}

bool Lexer::isDigit(char C) const {
  return C >= '0' && C <= '9';
}

bool Lexer::isAlnum(char C) const {
  return isAlpha(C) || isDigit(C);
}

TokenKind Lexer::identifyKeyword(const std::string& Text) {
  if (Text == "int") return TokenKind::KW_int;
  if (Text == "float") return TokenKind::KW_float;
  if (Text == "char") return TokenKind::KW_char;
  if (Text == "void") return TokenKind::KW_void;
  if (Text == "if") return TokenKind::KW_if;
  if (Text == "else") return TokenKind::KW_else;
  if (Text == "while") return TokenKind::KW_while;
  if (Text == "for") return TokenKind::KW_for;
  if (Text == "do") return TokenKind::KW_do;
  if (Text == "return") return TokenKind::KW_return;
  if (Text == "break") return TokenKind::KW_break;
  if (Text == "continue") return TokenKind::KW_continue;
  return TokenKind::Identifier;
}

} // namespace yac
