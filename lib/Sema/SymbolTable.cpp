#include "yac/Sema/SymbolTable.h"

namespace yac {

// ===----------------------------------------------------------------------===
// Scope implementation
// ===----------------------------------------------------------------------===

bool Scope::insert(std::unique_ptr<Symbol> Sym) {
  const std::string& Name = Sym->getName();

  // Check if symbol already exists in this scope
  if (Symbols.find(Name) != Symbols.end()) {
    return false;
  }

  // Insert into map and transfer ownership
  Symbol* RawPtr = Sym.get();
  OwnedSymbols.push_back(std::move(Sym));
  Symbols[Name] = RawPtr;
  return true;
}

Symbol* Scope::lookupLocal(const std::string& Name) {
  auto It = Symbols.find(Name);
  if (It != Symbols.end()) {
    return It->second;
  }
  return nullptr;
}

Symbol* Scope::lookup(const std::string& Name) {
  // First check this scope
  if (Symbol* Sym = lookupLocal(Name)) {
    return Sym;
  }

  // Then check parent scopes
  if (Parent) {
    return Parent->lookup(Name);
  }

  return nullptr;
}

// ===----------------------------------------------------------------------===
// SymbolTable implementation
// ===----------------------------------------------------------------------===

SymbolTable::SymbolTable() {
  GlobalScope = std::make_unique<Scope>(nullptr);
  CurrentScope = GlobalScope.get();
}

void SymbolTable::pushScope() {
  auto NewScope = std::make_unique<Scope>(CurrentScope);
  CurrentScope = NewScope.get();
  AllScopes.push_back(std::move(NewScope));
}

void SymbolTable::popScope() {
  if (CurrentScope && CurrentScope != GlobalScope.get()) {
    CurrentScope = CurrentScope->getParent();
  }
}

bool SymbolTable::insert(std::unique_ptr<Symbol> Sym) {
  return CurrentScope->insert(std::move(Sym));
}

Symbol* SymbolTable::lookup(const std::string& Name) {
  return CurrentScope->lookup(Name);
}

} // namespace yac
