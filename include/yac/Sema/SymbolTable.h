#ifndef YAC_SEMA_SYMBOLTABLE_H
#define YAC_SEMA_SYMBOLTABLE_H

#include "yac/AST/AST.h"
#include "yac/Type/Type.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace yac {

/// Symbol - represents a declared entity (variable, parameter, function)
class Symbol {
public:
  enum SymbolKind {
    SK_Variable,
    SK_Parameter,
    SK_Function
  };

private:
  SymbolKind Kind;
  std::string Name;
  Type* SymType;
  Decl* Declaration;  // Pointer to the AST node that declared this symbol

public:
  Symbol(SymbolKind K, std::string Name, Type* Ty, Decl* D)
      : Kind(K), Name(std::move(Name)), SymType(Ty), Declaration(D) {}

  SymbolKind getKind() const { return Kind; }
  const std::string& getName() const { return Name; }
  Type* getType() const { return SymType; }
  Decl* getDeclaration() const { return Declaration; }

  bool isVariable() const { return Kind == SK_Variable; }
  bool isParameter() const { return Kind == SK_Parameter; }
  bool isFunction() const { return Kind == SK_Function; }
};

/// Scope - represents a lexical scope (block, function, global)
class Scope {
  Scope* Parent;
  std::map<std::string, Symbol*> Symbols;
  std::vector<std::unique_ptr<Symbol>> OwnedSymbols;

public:
  Scope(Scope* Parent = nullptr) : Parent(Parent) {}

  /// Insert a symbol into this scope
  /// Returns true on success, false if symbol already exists
  bool insert(std::unique_ptr<Symbol> Sym);

  /// Lookup a symbol in this scope only (not parent scopes)
  Symbol* lookupLocal(const std::string& Name);

  /// Lookup a symbol in this scope and all parent scopes
  Symbol* lookup(const std::string& Name);

  /// Get parent scope
  Scope* getParent() const { return Parent; }

  /// Get all symbols in this scope
  const std::map<std::string, Symbol*>& getSymbols() const { return Symbols; }
};

/// SymbolTable - manages scopes and symbol resolution
class SymbolTable {
  std::unique_ptr<Scope> GlobalScope;
  Scope* CurrentScope;
  std::vector<std::unique_ptr<Scope>> AllScopes;

public:
  SymbolTable();

  /// Enter a new scope (e.g., entering a function or block)
  void pushScope();

  /// Exit current scope
  void popScope();

  /// Get current scope
  Scope* getCurrentScope() const { return CurrentScope; }

  /// Get global scope
  Scope* getGlobalScope() const { return GlobalScope.get(); }

  /// Insert a symbol into current scope
  bool insert(std::unique_ptr<Symbol> Sym);

  /// Lookup a symbol starting from current scope
  Symbol* lookup(const std::string& Name);

  /// Check if we're at global scope
  bool isGlobalScope() const { return CurrentScope == GlobalScope.get(); }
};

} // namespace yac

#endif // YAC_SEMA_SYMBOLTABLE_H
