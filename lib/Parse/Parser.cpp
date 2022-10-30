#include "yac/Parse/Parser.h"
#include <cassert>

namespace yac {

// ===----------------------------------------------------------------------===
// Token management
// ===----------------------------------------------------------------------===

const Token& Parser::peek(size_t Offset) const {
  size_t Idx = CurrentToken + Offset;
  if (Idx >= Tokens.size()) {
    return Tokens.back(); // Return EOF token
  }
  return Tokens[Idx];
}

bool Parser::expect(TokenKind K) {
  if (currentToken().is(K)) {
    return true;
  }
  error("Expected " + std::string(Token::getTokenKindName(K)) +
        ", got " + std::string(currentToken().getKindName()));
  return false;
}

bool Parser::consume(TokenKind K) {
  if (currentToken().is(K)) {
    advance();
    return true;
  }
  return false;
}

// ===----------------------------------------------------------------------===
// Error handling
// ===----------------------------------------------------------------------===

void Parser::error(const std::string& Msg) {
  error(currentToken().getLocation(), Msg);
}

void Parser::error(SourceLocation Loc, const std::string& Msg) {
  Diag.error(Loc, Msg);
}

// ===----------------------------------------------------------------------===
// Entry point
// ===----------------------------------------------------------------------===

std::unique_ptr<TranslationUnit> Parser::parseTranslationUnit() {
  auto TU = std::make_unique<TranslationUnit>();

  while (!isAtEnd()) {
    if (Decl* D = parseDeclaration()) {
      TU->addDecl(D);
    } else {
      // Skip to next declaration on error
      while (!isAtEnd() && !currentToken().is(TokenKind::Semicolon)) {
        advance();
      }
      if (currentToken().is(TokenKind::Semicolon)) {
        advance();
      }
    }
  }

  // Release ownership from pools - AST nodes now own each other
  for (auto& ptr : ExprPool) ptr.release();
  for (auto& ptr : StmtPool) ptr.release();
  for (auto& ptr : DeclPool) ptr.release();
  for (auto& ptr : VarDeclPool) ptr.release();
  for (auto& ptr : ParmDeclPool) ptr.release();

  return TU;
}

// ===----------------------------------------------------------------------===
// Type parsing
// ===----------------------------------------------------------------------===

Type* Parser::parseTypeSpecifier() {
  switch (currentToken().getKind()) {
  case TokenKind::KW_int:
    advance();
    return TyCtx.getIntType();
  case TokenKind::KW_float:
    advance();
    return TyCtx.getFloatType();
  case TokenKind::KW_char:
    advance();
    return TyCtx.getCharType();
  case TokenKind::KW_void:
    advance();
    return TyCtx.getVoidType();
  default:
    error("Expected type specifier");
    return nullptr;
  }
}

// ===----------------------------------------------------------------------===
// Declaration parsing
// ===----------------------------------------------------------------------===

Decl* Parser::parseDeclaration() {
  // Parse type specifier
  Type* DeclType = parseTypeSpecifier();
  if (!DeclType) return nullptr;

  // Expect identifier
  if (!currentToken().is(TokenKind::Identifier)) {
    error("Expected identifier");
    return nullptr;
  }

  std::string Name = currentToken().getText();
  SourceLocation Loc = currentToken().getLocation();
  advance();

  // Check if it's a function or variable
  if (currentToken().is(TokenKind::LParen)) {
    // Function declaration/definition
    return parseFunctionDeclaration(DeclType, Name);
  } else {
    // Variable declaration
    return parseVariableDeclaration(DeclType, Name);
  }
}

VarDecl* Parser::parseVariableDeclaration(Type* DeclType, const std::string& Name) {
  SourceLocation Loc = currentToken().getLocation();

  // Check for array
  if (currentToken().is(TokenKind::LBracket)) {
    advance();
    int Size = -1;
    if (currentToken().is(TokenKind::IntegerLiteral)) {
      Size = currentToken().getIntValue();
      advance();
    }
    expect(TokenKind::RBracket);
    advance();
    DeclType = TyCtx.getArrayType(DeclType, Size);
  }

  // Check for initializer
  std::unique_ptr<Expr> Init;
  if (consume(TokenKind::Equal)) {
    Init.reset(parseExpression());
  }

  expect(TokenKind::Semicolon);
  advance();

  return create<VarDecl>(SourceRange(Loc), Name, DeclType, std::move(Init));
}

FunctionDecl* Parser::parseFunctionDeclaration(Type* RetType, const std::string& Name) {
  SourceLocation Loc = currentToken().getLocation();

  expect(TokenKind::LParen);
  advance();

  auto Params = parseParameterList();

  expect(TokenKind::RParen);
  advance();

  // Check for function body
  std::unique_ptr<CompoundStmt> Body;
  if (currentToken().is(TokenKind::LBrace)) {
    Body.reset(parseCompoundStatement());
  } else {
    expect(TokenKind::Semicolon);
    advance();
  }

  return create<FunctionDecl>(SourceRange(Loc), Name, RetType, Params, std::move(Body));
}

std::vector<ParmVarDecl*> Parser::parseParameterList() {
  std::vector<ParmVarDecl*> Params;

  if (currentToken().is(TokenKind::RParen)) {
    return Params; // Empty parameter list
  }

  do {
    Type* ParamType = parseTypeSpecifier();
    if (!ParamType) break;

    std::string ParamName;
    if (currentToken().is(TokenKind::Identifier)) {
      ParamName = currentToken().getText();
      SourceLocation Loc = currentToken().getLocation();
      advance();

      // Check for array parameter
      if (consume(TokenKind::LBracket)) {
        expect(TokenKind::RBracket);
        advance();
        ParamType = TyCtx.getArrayType(ParamType);
      }

      Params.push_back(create<ParmVarDecl>(SourceRange(Loc), ParamName, ParamType));
    }

  } while (consume(TokenKind::Comma));

  return Params;
}

// ===----------------------------------------------------------------------===
// Statement parsing
// ===----------------------------------------------------------------------===

Stmt* Parser::parseStatement() {
  switch (currentToken().getKind()) {
  case TokenKind::LBrace:
    return parseCompoundStatement();
  case TokenKind::KW_if:
    return parseIfStatement();
  case TokenKind::KW_while:
    return parseWhileStatement();
  case TokenKind::KW_for:
    return parseForStatement();
  case TokenKind::KW_do:
    return parseDoStatement();
  case TokenKind::KW_return:
    return parseReturnStatement();
  case TokenKind::KW_break: {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    expect(TokenKind::Semicolon);
    advance();
    return create<BreakStmt>(SourceRange(Loc));
  }
  case TokenKind::KW_continue: {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    expect(TokenKind::Semicolon);
    advance();
    return create<ContinueStmt>(SourceRange(Loc));
  }
  // Variable declaration
  case TokenKind::KW_int:
  case TokenKind::KW_float:
  case TokenKind::KW_char:
  case TokenKind::KW_void: {
    Decl* D = parseDeclaration();
    if (D) {
      return create<DeclStmt>(D->getSourceRange(), D);
    }
    return nullptr;
  }
  default:
    return parseExpressionStatement();
  }
}

CompoundStmt* Parser::parseCompoundStatement() {
  SourceLocation Loc = currentToken().getLocation();
  expect(TokenKind::LBrace);
  advance();

  std::vector<std::unique_ptr<Stmt>> Stmts;
  while (!isAtEnd() && !currentToken().is(TokenKind::RBrace)) {
    if (Stmt* S = parseStatement()) {
      Stmts.push_back(std::unique_ptr<Stmt>(S));
    }
  }

  expect(TokenKind::RBrace);
  advance();

  return create<CompoundStmt>(SourceRange(Loc), std::move(Stmts));
}

Stmt* Parser::parseIfStatement() {
  SourceLocation Loc = currentToken().getLocation();
  advance(); // consume 'if'

  expect(TokenKind::LParen);
  advance();

  Expr* Cond = parseExpression();

  expect(TokenKind::RParen);
  advance();

  Stmt* Then = parseStatement();

  std::unique_ptr<Stmt> Else;
  if (consume(TokenKind::KW_else)) {
    Else.reset(parseStatement());
  }

  return create<IfStmt>(SourceRange(Loc), std::unique_ptr<Expr>(Cond),
                        std::unique_ptr<Stmt>(Then), std::move(Else));
}

Stmt* Parser::parseWhileStatement() {
  SourceLocation Loc = currentToken().getLocation();
  advance(); // consume 'while'

  expect(TokenKind::LParen);
  advance();

  Expr* Cond = parseExpression();

  expect(TokenKind::RParen);
  advance();

  Stmt* Body = parseStatement();

  return create<WhileStmt>(SourceRange(Loc), std::unique_ptr<Expr>(Cond),
                           std::unique_ptr<Stmt>(Body));
}

Stmt* Parser::parseForStatement() {
  SourceLocation Loc = currentToken().getLocation();
  advance(); // consume 'for'

  expect(TokenKind::LParen);
  advance();

  // Init
  std::unique_ptr<Stmt> Init;
  if (!currentToken().is(TokenKind::Semicolon)) {
    if (currentToken().isOneOf(TokenKind::KW_int, TokenKind::KW_float,
                                TokenKind::KW_char)) {
      Init.reset(parseStatement());
    } else {
      Init.reset(parseExpressionStatement());
    }
  } else {
    advance(); // consume semicolon
  }

  // Condition
  std::unique_ptr<Expr> Cond;
  if (!currentToken().is(TokenKind::Semicolon)) {
    Cond.reset(parseExpression());
  }
  expect(TokenKind::Semicolon);
  advance();

  // Increment
  std::unique_ptr<Expr> Inc;
  if (!currentToken().is(TokenKind::RParen)) {
    Inc.reset(parseExpression());
  }
  expect(TokenKind::RParen);
  advance();

  Stmt* Body = parseStatement();

  return create<ForStmt>(SourceRange(Loc), std::move(Init), std::move(Cond),
                         std::move(Inc), std::unique_ptr<Stmt>(Body));
}

Stmt* Parser::parseDoStatement() {
  SourceLocation Loc = currentToken().getLocation();
  advance(); // consume 'do'

  Stmt* Body = parseStatement();

  expect(TokenKind::KW_while);
  advance();

  expect(TokenKind::LParen);
  advance();

  Expr* Cond = parseExpression();

  expect(TokenKind::RParen);
  advance();

  expect(TokenKind::Semicolon);
  advance();

  return create<DoStmt>(SourceRange(Loc), std::unique_ptr<Stmt>(Body),
                        std::unique_ptr<Expr>(Cond));
}

Stmt* Parser::parseReturnStatement() {
  SourceLocation Loc = currentToken().getLocation();
  advance(); // consume 'return'

  std::unique_ptr<Expr> RetVal;
  if (!currentToken().is(TokenKind::Semicolon)) {
    RetVal.reset(parseExpression());
  }

  expect(TokenKind::Semicolon);
  advance();

  return create<ReturnStmt>(SourceRange(Loc), std::move(RetVal));
}

Stmt* Parser::parseExpressionStatement() {
  SourceLocation Loc = currentToken().getLocation();

  if (currentToken().is(TokenKind::Semicolon)) {
    advance();
    return create<ExprStmt>(SourceRange(Loc), nullptr);
  }

  Expr* E = parseExpression();
  expect(TokenKind::Semicolon);
  advance();

  return create<ExprStmt>(SourceRange(Loc), std::unique_ptr<Expr>(E));
}

// ===----------------------------------------------------------------------===
// Expression parsing (operator precedence climbing)
// ===----------------------------------------------------------------------===

Expr* Parser::parseExpression() {
  return parseAssignmentExpression();
}

Expr* Parser::parseAssignmentExpression() {
  Expr* LHS = parseLogicalOrExpression();

  if (currentToken().isOneOf(TokenKind::Equal, TokenKind::PlusEqual,
                              TokenKind::MinusEqual, TokenKind::StarEqual,
                              TokenKind::SlashEqual)) {
    TokenKind OpKind = currentToken().getKind();
    SourceLocation Loc = currentToken().getLocation();
    advance();

    Expr* RHS = parseAssignmentExpression();

    BinaryOperatorKind Op;
    if (OpKind == TokenKind::Equal)
      Op = BinaryOperatorKind::Assign;
    else if (OpKind == TokenKind::PlusEqual)
      Op = BinaryOperatorKind::AddAssign;
    else if (OpKind == TokenKind::MinusEqual)
      Op = BinaryOperatorKind::SubAssign;
    else if (OpKind == TokenKind::StarEqual)
      Op = BinaryOperatorKind::MulAssign;
    else
      Op = BinaryOperatorKind::DivAssign;

    return create<BinaryOperator>(SourceRange(Loc), Op,
                                  std::unique_ptr<Expr>(LHS),
                                  std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseLogicalOrExpression() {
  Expr* LHS = parseLogicalAndExpression();

  while (currentToken().is(TokenKind::PipePipe)) {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    Expr* RHS = parseLogicalAndExpression();
    LHS = create<BinaryOperator>(SourceRange(Loc), BinaryOperatorKind::LOr,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseLogicalAndExpression() {
  Expr* LHS = parseBitwiseOrExpression();

  while (currentToken().is(TokenKind::AmpAmp)) {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    Expr* RHS = parseBitwiseOrExpression();
    LHS = create<BinaryOperator>(SourceRange(Loc), BinaryOperatorKind::LAnd,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseBitwiseOrExpression() {
  Expr* LHS = parseBitwiseXorExpression();

  while (currentToken().is(TokenKind::Pipe)) {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    Expr* RHS = parseBitwiseXorExpression();
    LHS = create<BinaryOperator>(SourceRange(Loc), BinaryOperatorKind::Or,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseBitwiseXorExpression() {
  Expr* LHS = parseBitwiseAndExpression();

  while (currentToken().is(TokenKind::Caret)) {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    Expr* RHS = parseBitwiseAndExpression();
    LHS = create<BinaryOperator>(SourceRange(Loc), BinaryOperatorKind::Xor,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseBitwiseAndExpression() {
  Expr* LHS = parseEqualityExpression();

  while (currentToken().is(TokenKind::Amp)) {
    SourceLocation Loc = currentToken().getLocation();
    advance();
    Expr* RHS = parseEqualityExpression();
    LHS = create<BinaryOperator>(SourceRange(Loc), BinaryOperatorKind::And,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseEqualityExpression() {
  Expr* LHS = parseRelationalExpression();

  while (currentToken().isOneOf(TokenKind::EqualEqual, TokenKind::NotEqual)) {
    TokenKind OpKind = currentToken().getKind();
    SourceLocation Loc = currentToken().getLocation();
    advance();

    Expr* RHS = parseRelationalExpression();

    BinaryOperatorKind Op =
        OpKind == TokenKind::EqualEqual ? BinaryOperatorKind::EQ
                                        : BinaryOperatorKind::NE;

    LHS = create<BinaryOperator>(SourceRange(Loc), Op,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseRelationalExpression() {
  Expr* LHS = parseShiftExpression();

  while (currentToken().isOneOf(TokenKind::Less, TokenKind::Greater,
                                 TokenKind::LessEqual, TokenKind::GreaterEqual)) {
    TokenKind OpKind = currentToken().getKind();
    SourceLocation Loc = currentToken().getLocation();
    advance();

    Expr* RHS = parseShiftExpression();

    BinaryOperatorKind Op;
    if (OpKind == TokenKind::Less)
      Op = BinaryOperatorKind::LT;
    else if (OpKind == TokenKind::Greater)
      Op = BinaryOperatorKind::GT;
    else if (OpKind == TokenKind::LessEqual)
      Op = BinaryOperatorKind::LE;
    else
      Op = BinaryOperatorKind::GE;

    LHS = create<BinaryOperator>(SourceRange(Loc), Op,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseShiftExpression() {
  Expr* LHS = parseAdditiveExpression();

  while (currentToken().isOneOf(TokenKind::LessLess, TokenKind::GreaterGreater)) {
    TokenKind OpKind = currentToken().getKind();
    SourceLocation Loc = currentToken().getLocation();
    advance();

    Expr* RHS = parseAdditiveExpression();

    BinaryOperatorKind Op =
        OpKind == TokenKind::LessLess ? BinaryOperatorKind::Shl
                                      : BinaryOperatorKind::Shr;

    LHS = create<BinaryOperator>(SourceRange(Loc), Op,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseAdditiveExpression() {
  Expr* LHS = parseMultiplicativeExpression();

  while (currentToken().isOneOf(TokenKind::Plus, TokenKind::Minus)) {
    TokenKind OpKind = currentToken().getKind();
    SourceLocation Loc = currentToken().getLocation();
    advance();

    Expr* RHS = parseMultiplicativeExpression();

    BinaryOperatorKind Op =
        OpKind == TokenKind::Plus ? BinaryOperatorKind::Add
                                  : BinaryOperatorKind::Sub;

    LHS = create<BinaryOperator>(SourceRange(Loc), Op,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseMultiplicativeExpression() {
  Expr* LHS = parseUnaryExpression();

  while (currentToken().isOneOf(TokenKind::Star, TokenKind::Slash, TokenKind::Percent)) {
    TokenKind OpKind = currentToken().getKind();
    SourceLocation Loc = currentToken().getLocation();
    advance();

    Expr* RHS = parseUnaryExpression();

    BinaryOperatorKind Op;
    if (OpKind == TokenKind::Star)
      Op = BinaryOperatorKind::Mul;
    else if (OpKind == TokenKind::Slash)
      Op = BinaryOperatorKind::Div;
    else
      Op = BinaryOperatorKind::Mod;

    LHS = create<BinaryOperator>(SourceRange(Loc), Op,
                                 std::unique_ptr<Expr>(LHS),
                                 std::unique_ptr<Expr>(RHS));
  }

  return LHS;
}

Expr* Parser::parseUnaryExpression() {
  SourceLocation Loc = currentToken().getLocation();

  // Prefix operators
  if (currentToken().isOneOf(TokenKind::PlusPlus, TokenKind::MinusMinus)) {
    TokenKind OpKind = currentToken().getKind();
    advance();

    Expr* SubExpr = parseUnaryExpression();

    UnaryOperatorKind Op =
        OpKind == TokenKind::PlusPlus ? UnaryOperatorKind::PreInc
                                      : UnaryOperatorKind::PreDec;

    return create<UnaryOperator>(SourceRange(Loc), Op,
                                std::unique_ptr<Expr>(SubExpr));
  }

  if (currentToken().isOneOf(TokenKind::Plus, TokenKind::Minus,
                              TokenKind::Exclaim, TokenKind::Tilde,
                              TokenKind::Amp, TokenKind::Star)) {
    TokenKind OpKind = currentToken().getKind();
    advance();

    Expr* SubExpr = parseUnaryExpression();

    UnaryOperatorKind Op;
    if (OpKind == TokenKind::Plus)
      Op = UnaryOperatorKind::Plus;
    else if (OpKind == TokenKind::Minus)
      Op = UnaryOperatorKind::Minus;
    else if (OpKind == TokenKind::Exclaim)
      Op = UnaryOperatorKind::Not;
    else if (OpKind == TokenKind::Tilde)
      Op = UnaryOperatorKind::BitwiseNot;
    else if (OpKind == TokenKind::Amp)
      Op = UnaryOperatorKind::AddrOf;
    else
      Op = UnaryOperatorKind::Deref;

    return create<UnaryOperator>(SourceRange(Loc), Op,
                                std::unique_ptr<Expr>(SubExpr));
  }

  return parsePostfixExpression();
}

Expr* Parser::parsePostfixExpression() {
  Expr* LHS = parsePrimaryExpression();

  while (true) {
    SourceLocation Loc = currentToken().getLocation();

    // Postfix ++ and --
    if (currentToken().isOneOf(TokenKind::PlusPlus, TokenKind::MinusMinus)) {
      TokenKind OpKind = currentToken().getKind();
      advance();

      UnaryOperatorKind Op =
          OpKind == TokenKind::PlusPlus ? UnaryOperatorKind::PostInc
                                        : UnaryOperatorKind::PostDec;

      LHS = create<UnaryOperator>(SourceRange(Loc), Op,
                                  std::unique_ptr<Expr>(LHS));
      continue;
    }

    // Array subscript
    if (currentToken().is(TokenKind::LBracket)) {
      advance();
      Expr* Index = parseExpression();
      expect(TokenKind::RBracket);
      advance();

      LHS = create<ArraySubscriptExpr>(SourceRange(Loc),
                                       std::unique_ptr<Expr>(LHS),
                                       std::unique_ptr<Expr>(Index));
      continue;
    }

    // Function call
    if (currentToken().is(TokenKind::LParen)) {
      advance();

      std::vector<std::unique_ptr<Expr>> Args;
      if (!currentToken().is(TokenKind::RParen)) {
        do {
          Args.push_back(std::unique_ptr<Expr>(parseExpression()));
        } while (consume(TokenKind::Comma));
      }

      expect(TokenKind::RParen);
      advance();

      LHS = create<CallExpr>(SourceRange(Loc), std::unique_ptr<Expr>(LHS),
                            std::move(Args));
      continue;
    }

    break;
  }

  return LHS;
}

Expr* Parser::parsePrimaryExpression() {
  SourceLocation Loc = currentToken().getLocation();

  // Integer literal
  if (currentToken().is(TokenKind::IntegerLiteral)) {
    int64_t Val = currentToken().getIntValue();
    advance();
    return create<IntegerLiteral>(SourceRange(Loc), Val);
  }

  // Float literal
  if (currentToken().is(TokenKind::FloatLiteral)) {
    double Val = currentToken().getFloatValue();
    advance();
    return create<FloatLiteral>(SourceRange(Loc), Val);
  }

  // Char literal
  if (currentToken().is(TokenKind::CharLiteral)) {
    char Val = currentToken().getCharValue();
    advance();
    return create<CharLiteral>(SourceRange(Loc), Val);
  }

  // String literal
  if (currentToken().is(TokenKind::StringLiteral)) {
    std::string Val = currentToken().getText();
    advance();
    return create<StringLiteral>(SourceRange(Loc), Val);
  }

  // Identifier
  if (currentToken().is(TokenKind::Identifier)) {
    std::string Name = currentToken().getText();
    advance();
    return create<DeclRefExpr>(SourceRange(Loc), Name);
  }

  // Parenthesized expression
  if (currentToken().is(TokenKind::LParen)) {
    advance();
    Expr* E = parseExpression();
    expect(TokenKind::RParen);
    advance();
    return E;
  }

  error("Expected primary expression");
  return nullptr;
}

} // namespace yac
