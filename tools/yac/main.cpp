#include "yac/AST/AST.h"
#include "yac/AST/ASTVisitor.h"
#include "yac/Basic/Diagnostic.h"
#include "yac/Type/Type.h"
#include <iostream>
#include <fstream>

using namespace yac;

void printUsage(const char* progName) {
  std::cerr << "Usage: " << progName << " [options] <input-file>\n"
            << "Options:\n"
            << "  -h, --help           Show this help message\n"
            << "  -o <file>            Write output to <file>\n"
            << "  -S                   Emit assembly\n"
            << "  -c                   Compile to object file\n"
            << "  --dump-ast           Dump AST to stdout\n"
            << "  --dump-tokens        Dump tokens\n"
            << "  -fsyntax-only        Check syntax only\n"
            << "  -O<level>            Optimization level (0-3)\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printUsage(argv[0]);
    return 1;
  }

  // Parse command line arguments
  std::string inputFile;
  std::string outputFile;
  bool dumpAST = false;           // TODO: Implement --dump-ast
  bool syntaxOnly = false;        // TODO: Implement -fsyntax-only
  (void)dumpAST;                  // Suppress unused warning for now
  (void)syntaxOnly;               // Suppress unused warning for now

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      printUsage(argv[0]);
      return 0;
    } else if (arg == "--dump-ast") {
      dumpAST = true;
    } else if (arg == "-fsyntax-only") {
      syntaxOnly = true;
    } else if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg[0] != '-') {
      inputFile = arg;
    }
  }

  if (inputFile.empty()) {
    std::cerr << "Error: No input file specified\n";
    printUsage(argv[0]);
    return 1;
  }

  // Check if file exists
  std::ifstream test(inputFile);
  if (!test.good()) {
    std::cerr << "Error: Cannot open input file: " << inputFile << "\n";
    return 1;
  }
  test.close();

  std::cout << "YAC Compiler v0.2.0\n";
  std::cout << "Compiling: " << inputFile << "\n";

  // Initialize diagnostic engine
  DiagnosticEngine Diag;
  Diag.setUseColors(true);

  // Initialize type context
  TypeContext TyCtx;

  // TODO: Parse the input file
  std::cout << "Note: Parser not yet implemented. This is a skeleton.\n";
  std::cout << "Architecture setup complete:\n";
  std::cout << "  - AST nodes defined\n";
  std::cout << "  - Type system initialized\n";
  std::cout << "  - Diagnostic engine ready\n";

  // Demonstrate the system works
  SourceLocation Loc(1, 1, inputFile.c_str());
  Diag.note(Loc, "Compilation system initialized successfully");

  // Demonstrate AST visitor with a simple example
  std::cout << "\n--- Demonstrating AST Infrastructure ---\n";

  // Build a simple AST: int x = 42;
  auto* IntTy = TyCtx.getIntType();
  auto Init = std::make_unique<IntegerLiteral>(SourceRange(Loc), 42);
  Init->setType(IntTy);

  VarDecl VarX(SourceRange(Loc), "x", IntTy, std::move(Init));

  // Print the AST
  std::cout << "\nSample AST (int x = 42;):\n";
  ASTPrinter Printer(std::cout);
  Printer.visitVarDecl(&VarX);

  std::cout << "\nNext steps:\n";
  std::cout << "  1. Integrate Flex/Bison parser\n";
  std::cout << "  2. Build complete AST from parsed input\n";
  std::cout << "  3. Implement semantic analysis\n";
  std::cout << "  4. Add code generation backend\n";

  return 0;
}
