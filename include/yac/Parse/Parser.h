#ifndef YAC_PARSE_PARSER_H
#define YAC_PARSE_PARSER_H

#include "yac/AST/AST.h"
#include "yac/Basic/Diagnostic.h"
#include "yac/Parse/Token.h"
#include "yac/Type/Type.h"
#include <memory>
#include <vector>

namespace yac {

/// Parser - parses tokens and builds AST
class Parser {
  std::vector<Token> Tokens;
  size_t CurrentToken = 0;
  DiagnosticEngine& Diag;
  TypeContext& TyCtx;

  // Memory management
  std::vector<std::unique_ptr<Expr>> ExprPool;
  std::vector<std::unique_ptr<Stmt>> StmtPool;
  std::vector<std::unique_ptr<Decl>> DeclPool;
  std::vector<std::unique_ptr<VarDecl>> VarDeclPool;
  std::vector<std::unique_ptr<ParmVarDecl>> ParmDeclPool;

public:
  Parser(std::vector<Token> Tokens, DiagnosticEngine& Diag, TypeContext& TyCtx)
      : Tokens(std::move(Tokens)), Diag(Diag), TyCtx(TyCtx) {}

  // Parse entry point
  std::unique_ptr<TranslationUnit> parseTranslationUnit();

private:
  // Token management
  const Token& currentToken() const {
    if (CurrentToken >= Tokens.size() && !Tokens.empty()) {
      return Tokens.back(); // Return EOF token
    }
    return Tokens[CurrentToken];
  }
  const Token& peek(size_t Offset = 1) const;
  bool expect(TokenKind K);
  bool consume(TokenKind K);
  void advance() { if (CurrentToken < Tokens.size()) CurrentToken++; }
  bool isAtEnd() const { return CurrentToken >= Tokens.size() || currentToken().is(TokenKind::Eof); }

  // Error handling
  void error(const std::string& Msg);
  void error(SourceLocation Loc, const std::string& Msg);

  // Parsing methods - Declarations
  Decl* parseDeclaration();
  VarDecl* parseVariableDeclaration(Type* DeclType, const std::string& Name);
  FunctionDecl* parseFunctionDeclaration(Type* RetType, const std::string& Name);
  std::vector<ParmVarDecl*> parseParameterList();

  // Parsing methods - Statements
  Stmt* parseStatement();
  CompoundStmt* parseCompoundStatement();
  Stmt* parseIfStatement();
  Stmt* parseWhileStatement();
  Stmt* parseForStatement();
  Stmt* parseDoStatement();
  Stmt* parseReturnStatement();
  Stmt* parseExpressionStatement();

  // Parsing methods - Expressions (operator precedence)
  Expr* parseExpression();
  Expr* parseAssignmentExpression();
  Expr* parseLogicalOrExpression();
  Expr* parseLogicalAndExpression();
  Expr* parseBitwiseOrExpression();
  Expr* parseBitwiseXorExpression();
  Expr* parseBitwiseAndExpression();
  Expr* parseEqualityExpression();
  Expr* parseRelationalExpression();
  Expr* parseShiftExpression();
  Expr* parseAdditiveExpression();
  Expr* parseMultiplicativeExpression();
  Expr* parseUnaryExpression();
  Expr* parsePostfixExpression();
  Expr* parsePrimaryExpression();

  // Type parsing
  Type* parseTypeSpecifier();

  // Memory management helpers
  template<typename T, typename... Args>
  T* create(Args&&... args) {
    if constexpr (std::is_base_of_v<Expr, T>) {
      ExprPool.push_back(std::make_unique<T>(std::forward<Args>(args)...));
      return static_cast<T*>(ExprPool.back().get());
    } else if constexpr (std::is_base_of_v<Stmt, T>) {
      StmtPool.push_back(std::make_unique<T>(std::forward<Args>(args)...));
      return static_cast<T*>(StmtPool.back().get());
    } else if constexpr (std::is_base_of_v<VarDecl, T> && !std::is_same_v<VarDecl, T>) {
      ParmDeclPool.push_back(std::make_unique<T>(std::forward<Args>(args)...));
      return static_cast<T*>(ParmDeclPool.back().get());
    } else if constexpr (std::is_same_v<VarDecl, T>) {
      VarDeclPool.push_back(std::make_unique<T>(std::forward<Args>(args)...));
      return static_cast<T*>(VarDeclPool.back().get());
    } else {
      DeclPool.push_back(std::make_unique<T>(std::forward<Args>(args)...));
      return static_cast<T*>(DeclPool.back().get());
    }
  }
};

} // namespace yac

#endif // YAC_PARSE_PARSER_H
