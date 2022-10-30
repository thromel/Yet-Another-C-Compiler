#ifndef YAC_AST_AST_H
#define YAC_AST_AST_H

#include "yac/Basic/SourceLocation.h"
#include <memory>
#include <string>
#include <vector>

namespace yac {

// Forward declarations
class Type;
class ASTVisitor;

// ===----------------------------------------------------------------------===
// Base AST Node
// ===----------------------------------------------------------------------===

class ASTNode {
  SourceRange Loc;

protected:
  ASTNode(SourceRange Loc) : Loc(Loc) {}

public:
  virtual ~ASTNode() = default;

  SourceRange getSourceRange() const { return Loc; }
  SourceLocation getLocation() const { return Loc.getBegin(); }

  // RTTI support
  enum NodeKind {
    // Expressions
    NK_IntegerLiteral,
    NK_FloatLiteral,
    NK_CharLiteral,
    NK_StringLiteral,
    NK_DeclRefExpr,
    NK_BinaryOperator,
    NK_UnaryOperator,
    NK_CallExpr,
    NK_ArraySubscript,
    NK_ImplicitCastExpr,
    // Statements
    NK_CompoundStmt,
    NK_DeclStmt,
    NK_ExprStmt,
    NK_ReturnStmt,
    NK_IfStmt,
    NK_WhileStmt,
    NK_ForStmt,
    NK_DoStmt,
    NK_BreakStmt,
    NK_ContinueStmt,
    // Declarations
    NK_VarDecl,
    NK_ParmVarDecl,
    NK_FunctionDecl,
    NK_TranslationUnit
  };

  virtual NodeKind getKind() const = 0;
};

// ===----------------------------------------------------------------------===
// Expressions
// ===----------------------------------------------------------------------===

class Expr : public ASTNode {
protected:
  Type* ExprType = nullptr;

  Expr(SourceRange Loc) : ASTNode(Loc) {}

public:
  Type* getType() const { return ExprType; }
  void setType(Type* T) { ExprType = T; }

  static bool classof(const ASTNode* N) {
    return N->getKind() >= NK_IntegerLiteral &&
           N->getKind() <= NK_ImplicitCastExpr;
  }
};

class IntegerLiteral : public Expr {
  int64_t Value;

public:
  IntegerLiteral(SourceRange Loc, int64_t Value)
      : Expr(Loc), Value(Value) {}

  int64_t getValue() const { return Value; }

  NodeKind getKind() const override { return NK_IntegerLiteral; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_IntegerLiteral;
  }
};

class FloatLiteral : public Expr {
  double Value;

public:
  FloatLiteral(SourceRange Loc, double Value)
      : Expr(Loc), Value(Value) {}

  double getValue() const { return Value; }

  NodeKind getKind() const override { return NK_FloatLiteral; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_FloatLiteral;
  }
};

class CharLiteral : public Expr {
  char Value;

public:
  CharLiteral(SourceRange Loc, char Value)
      : Expr(Loc), Value(Value) {}

  char getValue() const { return Value; }

  NodeKind getKind() const override { return NK_CharLiteral; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_CharLiteral;
  }
};

class StringLiteral : public Expr {
  std::string Value;

public:
  StringLiteral(SourceRange Loc, std::string Value)
      : Expr(Loc), Value(std::move(Value)) {}

  const std::string& getValue() const { return Value; }

  NodeKind getKind() const override { return NK_StringLiteral; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_StringLiteral;
  }
};

class DeclRefExpr : public Expr {
  class VarDecl* Decl;
  std::string Name;

public:
  DeclRefExpr(SourceRange Loc, std::string Name)
      : Expr(Loc), Decl(nullptr), Name(std::move(Name)) {}

  const std::string& getName() const { return Name; }
  VarDecl* getDecl() const { return Decl; }
  void setDecl(VarDecl* D) { Decl = D; }

  NodeKind getKind() const override { return NK_DeclRefExpr; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_DeclRefExpr;
  }
};

enum class BinaryOperatorKind {
  // Arithmetic
  Add, Sub, Mul, Div, Mod,
  // Relational
  LT, GT, LE, GE, EQ, NE,
  // Logical
  LAnd, LOr,
  // Bitwise
  And, Or, Xor, Shl, Shr,
  // Assignment
  Assign,
  AddAssign, SubAssign, MulAssign, DivAssign, ModAssign
};

class BinaryOperator : public Expr {
  BinaryOperatorKind Op;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;

public:
  BinaryOperator(SourceRange Loc, BinaryOperatorKind Op,
                 std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS)
      : Expr(Loc), Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  BinaryOperatorKind getOp() const { return Op; }
  Expr* getLHS() const { return LHS.get(); }
  Expr* getRHS() const { return RHS.get(); }

  static const char* getOpName(BinaryOperatorKind Op);
  const char* getOpName() const { return getOpName(Op); }

  NodeKind getKind() const override { return NK_BinaryOperator; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_BinaryOperator;
  }
};

enum class UnaryOperatorKind {
  Plus, Minus, Not, BitwiseNot,
  PreInc, PreDec, PostInc, PostDec,
  AddrOf, Deref
};

class UnaryOperator : public Expr {
  UnaryOperatorKind Op;
  std::unique_ptr<Expr> SubExpr;

public:
  UnaryOperator(SourceRange Loc, UnaryOperatorKind Op,
                std::unique_ptr<Expr> SubExpr)
      : Expr(Loc), Op(Op), SubExpr(std::move(SubExpr)) {}

  UnaryOperatorKind getOp() const { return Op; }
  Expr* getSubExpr() const { return SubExpr.get(); }

  static const char* getOpName(UnaryOperatorKind Op);
  const char* getOpName() const { return getOpName(Op); }

  NodeKind getKind() const override { return NK_UnaryOperator; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_UnaryOperator;
  }
};

class CallExpr : public Expr {
  std::unique_ptr<Expr> Callee;
  std::vector<std::unique_ptr<Expr>> Args;

public:
  CallExpr(SourceRange Loc, std::unique_ptr<Expr> Callee,
           std::vector<std::unique_ptr<Expr>> Args)
      : Expr(Loc), Callee(std::move(Callee)), Args(std::move(Args)) {}

  Expr* getCallee() const { return Callee.get(); }
  size_t getNumArgs() const { return Args.size(); }
  Expr* getArg(size_t i) const { return Args[i].get(); }
  const std::vector<std::unique_ptr<Expr>>& getArgs() const { return Args; }

  NodeKind getKind() const override { return NK_CallExpr; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_CallExpr;
  }
};

class ArraySubscriptExpr : public Expr {
  std::unique_ptr<Expr> Base;
  std::unique_ptr<Expr> Index;

public:
  ArraySubscriptExpr(SourceRange Loc, std::unique_ptr<Expr> Base,
                     std::unique_ptr<Expr> Index)
      : Expr(Loc), Base(std::move(Base)), Index(std::move(Index)) {}

  Expr* getBase() const { return Base.get(); }
  Expr* getIndex() const { return Index.get(); }

  NodeKind getKind() const override { return NK_ArraySubscript; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ArraySubscript;
  }
};

enum class CastKind {
  IntToFloat,
  FloatToInt,
  IntToChar,
  CharToInt,
  ArrayToPointer,
  NoOp
};

class ImplicitCastExpr : public Expr {
  CastKind Kind;
  std::unique_ptr<Expr> SubExpr;

public:
  ImplicitCastExpr(SourceRange Loc, CastKind Kind,
                   std::unique_ptr<Expr> SubExpr, Type* DestType)
      : Expr(Loc), Kind(Kind), SubExpr(std::move(SubExpr)) {
    setType(DestType);
  }

  CastKind getCastKind() const { return Kind; }
  Expr* getSubExpr() const { return SubExpr.get(); }

  NodeKind getKind() const override { return NK_ImplicitCastExpr; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ImplicitCastExpr;
  }
};

// ===----------------------------------------------------------------------===
// Statements
// ===----------------------------------------------------------------------===

class Stmt : public ASTNode {
protected:
  Stmt(SourceRange Loc) : ASTNode(Loc) {}

public:
  static bool classof(const ASTNode* N) {
    return N->getKind() >= NK_CompoundStmt &&
           N->getKind() <= NK_ContinueStmt;
  }
};

class CompoundStmt : public Stmt {
  std::vector<std::unique_ptr<Stmt>> Stmts;

public:
  CompoundStmt(SourceRange Loc, std::vector<std::unique_ptr<Stmt>> Stmts)
      : Stmt(Loc), Stmts(std::move(Stmts)) {}

  size_t size() const { return Stmts.size(); }
  Stmt* operator[](size_t i) const { return Stmts[i].get(); }
  const std::vector<std::unique_ptr<Stmt>>& getStmts() const { return Stmts; }

  NodeKind getKind() const override { return NK_CompoundStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_CompoundStmt;
  }
};

class DeclStmt : public Stmt {
  class Decl* Declaration;

public:
  DeclStmt(SourceRange Loc, Decl* D) : Stmt(Loc), Declaration(D) {}

  Decl* getDecl() const { return Declaration; }

  NodeKind getKind() const override { return NK_DeclStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_DeclStmt;
  }
};

class ExprStmt : public Stmt {
  std::unique_ptr<Expr> Expression;

public:
  ExprStmt(SourceRange Loc, std::unique_ptr<Expr> E)
      : Stmt(Loc), Expression(std::move(E)) {}

  Expr* getExpr() const { return Expression.get(); }

  NodeKind getKind() const override { return NK_ExprStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ExprStmt;
  }
};

class ReturnStmt : public Stmt {
  std::unique_ptr<Expr> RetValue;

public:
  ReturnStmt(SourceRange Loc, std::unique_ptr<Expr> RetValue = nullptr)
      : Stmt(Loc), RetValue(std::move(RetValue)) {}

  Expr* getRetValue() const { return RetValue.get(); }
  bool hasRetValue() const { return RetValue != nullptr; }

  NodeKind getKind() const override { return NK_ReturnStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ReturnStmt;
  }
};

class IfStmt : public Stmt {
  std::unique_ptr<Expr> Condition;
  std::unique_ptr<Stmt> ThenStmt;
  std::unique_ptr<Stmt> ElseStmt;

public:
  IfStmt(SourceRange Loc, std::unique_ptr<Expr> Cond,
         std::unique_ptr<Stmt> Then, std::unique_ptr<Stmt> Else = nullptr)
      : Stmt(Loc), Condition(std::move(Cond)), ThenStmt(std::move(Then)),
        ElseStmt(std::move(Else)) {}

  Expr* getCondition() const { return Condition.get(); }
  Stmt* getThen() const { return ThenStmt.get(); }
  Stmt* getElse() const { return ElseStmt.get(); }
  bool hasElse() const { return ElseStmt != nullptr; }

  NodeKind getKind() const override { return NK_IfStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_IfStmt;
  }
};

class WhileStmt : public Stmt {
  std::unique_ptr<Expr> Condition;
  std::unique_ptr<Stmt> Body;

public:
  WhileStmt(SourceRange Loc, std::unique_ptr<Expr> Cond,
            std::unique_ptr<Stmt> Body)
      : Stmt(Loc), Condition(std::move(Cond)), Body(std::move(Body)) {}

  Expr* getCondition() const { return Condition.get(); }
  Stmt* getBody() const { return Body.get(); }

  NodeKind getKind() const override { return NK_WhileStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_WhileStmt;
  }
};

class ForStmt : public Stmt {
  std::unique_ptr<Stmt> Init;
  std::unique_ptr<Expr> Condition;
  std::unique_ptr<Expr> Increment;
  std::unique_ptr<Stmt> Body;

public:
  ForStmt(SourceRange Loc, std::unique_ptr<Stmt> Init,
          std::unique_ptr<Expr> Cond, std::unique_ptr<Expr> Inc,
          std::unique_ptr<Stmt> Body)
      : Stmt(Loc), Init(std::move(Init)), Condition(std::move(Cond)),
        Increment(std::move(Inc)), Body(std::move(Body)) {}

  Stmt* getInit() const { return Init.get(); }
  Expr* getCondition() const { return Condition.get(); }
  Expr* getIncrement() const { return Increment.get(); }
  Stmt* getBody() const { return Body.get(); }

  NodeKind getKind() const override { return NK_ForStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ForStmt;
  }
};

class DoStmt : public Stmt {
  std::unique_ptr<Stmt> Body;
  std::unique_ptr<Expr> Condition;

public:
  DoStmt(SourceRange Loc, std::unique_ptr<Stmt> Body,
         std::unique_ptr<Expr> Cond)
      : Stmt(Loc), Body(std::move(Body)), Condition(std::move(Cond)) {}

  Stmt* getBody() const { return Body.get(); }
  Expr* getCondition() const { return Condition.get(); }

  NodeKind getKind() const override { return NK_DoStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_DoStmt;
  }
};

class BreakStmt : public Stmt {
public:
  BreakStmt(SourceRange Loc) : Stmt(Loc) {}

  NodeKind getKind() const override { return NK_BreakStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_BreakStmt;
  }
};

class ContinueStmt : public Stmt {
public:
  ContinueStmt(SourceRange Loc) : Stmt(Loc) {}

  NodeKind getKind() const override { return NK_ContinueStmt; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ContinueStmt;
  }
};

// ===----------------------------------------------------------------------===
// Declarations
// ===----------------------------------------------------------------------===

class Decl : public ASTNode {
protected:
  Decl(SourceRange Loc) : ASTNode(Loc) {}

public:
  static bool classof(const ASTNode* N) {
    return N->getKind() >= NK_VarDecl &&
           N->getKind() <= NK_TranslationUnit;
  }
};

class VarDecl : public Decl {
  std::string Name;
  Type* DeclType;
  std::unique_ptr<Expr> Init;
  bool IsArray = false;
  int ArraySize = 0;

public:
  VarDecl(SourceRange Loc, std::string Name, Type* DeclType,
          std::unique_ptr<Expr> Init = nullptr)
      : Decl(Loc), Name(std::move(Name)), DeclType(DeclType),
        Init(std::move(Init)) {}

  const std::string& getName() const { return Name; }
  Type* getType() const { return DeclType; }
  void setType(Type* T) { DeclType = T; }

  Expr* getInit() const { return Init.get(); }
  bool hasInit() const { return Init != nullptr; }

  void setIsArray(bool IsArr, int Size = 0) {
    IsArray = IsArr;
    ArraySize = Size;
  }
  bool isArray() const { return IsArray; }
  int getArraySize() const { return ArraySize; }

  NodeKind getKind() const override { return NK_VarDecl; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_VarDecl || N->getKind() == NK_ParmVarDecl;
  }
};

class ParmVarDecl : public VarDecl {
public:
  ParmVarDecl(SourceRange Loc, std::string Name, Type* DeclType)
      : VarDecl(Loc, std::move(Name), DeclType) {}

  NodeKind getKind() const override { return NK_ParmVarDecl; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_ParmVarDecl;
  }
};

class FunctionDecl : public Decl {
  std::string Name;
  Type* ReturnType;
  std::vector<ParmVarDecl*> Params;
  std::unique_ptr<CompoundStmt> Body;
  bool IsDefined = false;

public:
  FunctionDecl(SourceRange Loc, std::string Name, Type* ReturnType,
               std::vector<ParmVarDecl*> Params,
               std::unique_ptr<CompoundStmt> Body = nullptr)
      : Decl(Loc), Name(std::move(Name)), ReturnType(ReturnType),
        Params(std::move(Params)), Body(std::move(Body)) {
    IsDefined = (Body != nullptr);
  }

  const std::string& getName() const { return Name; }
  Type* getReturnType() const { return ReturnType; }

  size_t getNumParams() const { return Params.size(); }
  ParmVarDecl* getParam(size_t i) const { return Params[i]; }
  const std::vector<ParmVarDecl*>& getParams() const { return Params; }

  CompoundStmt* getBody() const { return Body.get(); }
  bool hasBody() const { return Body != nullptr; }
  bool isDefined() const { return IsDefined; }

  void setBody(std::unique_ptr<CompoundStmt> B) {
    Body = std::move(B);
    IsDefined = true;
  }

  NodeKind getKind() const override { return NK_FunctionDecl; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_FunctionDecl;
  }
};

class TranslationUnit : public Decl {
  std::vector<Decl*> Declarations;

public:
  TranslationUnit() : Decl(SourceRange()) {}

  void addDecl(Decl* D) { Declarations.push_back(D); }

  size_t size() const { return Declarations.size(); }
  Decl* operator[](size_t i) const { return Declarations[i]; }
  const std::vector<Decl*>& getDecls() const { return Declarations; }

  NodeKind getKind() const override { return NK_TranslationUnit; }
  static bool classof(const ASTNode* N) {
    return N->getKind() == NK_TranslationUnit;
  }
};

} // namespace yac

#endif // YAC_AST_AST_H
