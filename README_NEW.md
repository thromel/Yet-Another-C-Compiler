# Yet Another C Compiler (YAC) - Modernized Architecture

A complete C compiler with modern architecture, featuring clean phase separation, comprehensive AST, type system, and LLVM backend support.

## 🎯 Project Status

**Phase 1 Complete**: Foundation & Architecture ✅

We've successfully refactored the compiler from a single-pass design to a modern, maintainable multi-pass architecture:

- ✅ Clean project structure (include/, lib/, tools/, test/)
- ✅ CMake-based build system
- ✅ Comprehensive AST node hierarchy
- ✅ Type system with context management
- ✅ Diagnostic engine with colored output
- ✅ Source location tracking
- ✅ Test infrastructure (unit & integration)
- ⏳ Parser integration (in progress)
- ⏳ Semantic analysis (in progress)
- ⏳ LLVM backend (planned)

## 📁 Architecture

```
Yet-Another-C-Compiler/
├── include/yac/          # Public headers
│   ├── AST/             # Abstract Syntax Tree nodes
│   ├── Basic/           # Source locations, diagnostics
│   ├── Type/            # Type system
│   ├── Sema/            # Semantic analysis (planned)
│   └── Codegen/         # Code generation (planned)
├── lib/                 # Implementation
│   ├── AST/
│   ├── Basic/
│   ├── Type/
│   ├── Sema/            # (planned)
│   └── Codegen/         # (planned)
├── tools/yac/           # Compiler driver
├── test/                # Tests
│   ├── unit/           # Unit tests (GoogleTest)
│   ├── integration/    # End-to-end tests
│   └── fixtures/       # Test input files
├── legacy/              # Original implementation (reference)
└── docs/                # Documentation
```

## 🏗️ Compilation Pipeline (Target)

```
Source Code
    ↓
[Lexer] (Flex)
    ↓
[Parser] (Bison) → Build AST
    ↓
[Semantic Analysis] → Type check, resolve symbols
    ↓
[HIR] (High-level IR) → Desugared, typed IR
    ↓
[Optimizations] → Constant folding, DCE, etc.
    ↓
[Code Generation]
    ├─→ [8086 Backend] (legacy)
    └─→ [LLVM Backend] → x86-64, ARM64, WebAssembly
```

## 🚀 Quick Start

### Build Requirements

- CMake 3.20+
- C++17 compiler (GCC, Clang, or MSVC)
- Flex (optional, for parser generation)
- Bison 3.0+ (optional, for parser generation)
- GoogleTest (optional, for unit tests)
- LLVM 14+ (optional, for LLVM backend)

### Building

```bash
# Configure
mkdir build && cd build
cmake .. -DYAC_ENABLE_LLVM=OFF

# Build
make -j$(nproc)

# Run compiler
./tools/yac ../test/fixtures/simple.c

# Run tests (if GoogleTest is installed)
ctest --output-on-failure
```

### Build Options

```bash
cmake .. \
  -DYAC_BUILD_TESTS=ON          # Build tests (default: ON)
  -DYAC_ENABLE_LLVM=ON           # Enable LLVM backend (default: ON)
  -DYAC_ENABLE_ASAN=OFF          # AddressSanitizer (default: OFF)
  -DYAC_ENABLE_UBSAN=OFF         # UndefinedBehaviorSanitizer (default: OFF)
```

## 📚 Design Highlights

### AST Design

Clean, well-structured AST with proper inheritance hierarchy:

```cpp
ASTNode (base)
├── Expr (expressions)
│   ├── IntegerLiteral, FloatLiteral, StringLiteral, CharLiteral
│   ├── DeclRefExpr (variable/function references)
│   ├── BinaryOperator (arithmetic, logical, bitwise, assignment)
│   ├── UnaryOperator (prefix/postfix inc/dec, unary +/-, !, ~, &, *)
│   ├── CallExpr (function calls)
│   ├── ArraySubscriptExpr
│   └── ImplicitCastExpr
├── Stmt (statements)
│   ├── CompoundStmt, DeclStmt, ExprStmt, ReturnStmt
│   ├── IfStmt, WhileStmt, ForStmt, DoStmt
│   └── BreakStmt, ContinueStmt
└── Decl (declarations)
    ├── VarDecl (variables)
    ├── ParmVarDecl (function parameters)
    ├── FunctionDecl (functions)
    └── TranslationUnit (top-level)
```

### Type System

Robust type system with canonical types:

```cpp
Type (base)
├── VoidType
├── IntType
├── FloatType
├── CharType
├── PointerType → Type*
├── ArrayType → Type[size]
└── FunctionType → RetType(ParamTypes...)
```

**Features**:
- Type equality checking
- Implicit conversion compatibility
- Type context for managing unique type instances
- Array-to-pointer decay
- Function type signatures

### Diagnostics

Clang-style diagnostics with source locations:

```cpp
DiagnosticEngine diag;
diag.error(loc, "undefined variable 'x'");
diag.warning(loc, "unused variable 'y'");
diag.note(loc, "declared here");
```

**Output**:
```
test.c:10:5: error: undefined variable 'x'
test.c:5:9: note: declared here
```

## 🎓 Next Steps

### Phase 2: Parser Integration

1. **Update Flex lexer**
   - Reentrant scanner
   - Location tracking
   - Token attribution

2. **Update Bison parser**
   - C++ skeleton (`%skeleton "lalr1.cc"`)
   - Build AST instead of generating code
   - Proper error recovery
   - Location tracking (`%locations`)

3. **AST Visitor Pattern**
   - Traversal infrastructure
   - Pretty-printer
   - DOT graph exporter

### Phase 3: Semantic Analysis

1. **Symbol Table**
   - Scope management
   - Symbol resolution
   - Function signature checking

2. **Type Checker**
   - Expression type inference
   - Implicit conversions
   - Lvalue/rvalue analysis

3. **Semantic Checks**
   - Undefined variable detection
   - Function call validation
   - Array bounds (static)
   - Control flow analysis

### Phase 4: Code Generation

1. **LLVM Backend**
   - Lower AST to LLVM IR
   - Register allocation
   - Optimization passes (-O0 through -O3)
   - Multiple targets (x86-64, ARM64, WebAssembly)

2. **Legacy 8086 Backend** (optional)
   - Keep as `-march=8086` for compatibility
   - Demonstrate multi-backend architecture

## 🧪 Testing

### Unit Tests

```bash
cd build
./test/unit/yac_unit_tests
```

Tests cover:
- AST node construction
- Type system operations
- Diagnostic engine
- Source location tracking

### Integration Tests

```bash
cd build
ctest -R IntegrationTests
```

End-to-end compiler tests with fixture files.

## 🎨 Code Style

- C++17 standard
- LLVM coding conventions
- `CamelCase` for types
- `camelCase` for functions/variables
- Smart pointers for ownership
- `const` correctness

## 📖 Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Compiler architecture overview
- [PROJECT_OVERVIEW.md](docs/PROJECT_OVERVIEW.md) - Development guide
- [deep-dive.md](docs/deep-dive.md) - Technical details

## 🚧 Migration from Legacy

The original single-pass compiler has been moved to `legacy/`:
- `legacy/1705069.l` - Original Flex lexer
- `legacy/1705069.y` - Original Bison parser (with inline codegen)
- `legacy/ParserUtils.h` - Mixed parser/codegen utilities
- `legacy/asmUtils.h` - 8086 assembly generation
- `legacy/SymbolTable/` - Original symbol table

**Key improvements in new architecture**:
- **Separation of concerns**: Parse → AST → Semantic → Codegen
- **Testability**: Each phase can be tested independently
- **Maintainability**: Clean interfaces, no global state
- **Extensibility**: Easy to add new backends, optimizations, language features
- **Diagnostics**: Professional error messages with locations

## 📊 Project Statistics

| Metric | Legacy | New |
|--------|--------|-----|
| Architecture | Single-pass | Multi-pass |
| Lines of Code | ~1,900 | ~2,500 |
| Files | 5 | 25+ |
| Tests | Manual | Automated |
| Backends | 8086 only | LLVM + 8086 |
| Global State | Heavy | Minimal |
| Type Safety | Mixed | Strong |

## 🤝 Contributing

This is an educational project demonstrating modern compiler construction. Feel free to:

1. Add more language features (structs, unions, enums, typedefs)
2. Implement optimization passes
3. Add better error recovery
4. Improve diagnostics (fix-it hints, did-you-mean)
5. Add LSP server for editor integration

## 📄 License

MIT License - See LICENSE file for details

## 👥 Author

**Romel**
Department of Computer Science and Engineering
Bangladesh University of Engineering and Technology

## 🙏 Acknowledgments

- Built with Flex and Bison
- LLVM for backend infrastructure
- Inspired by Clang's architecture
- Course: CSE 309 - Compilers

---

**Status**: Phase 1 (Foundation) ✅ Complete | Phase 2 (Parser) 🚧 Next

**Last Updated**: 2025
