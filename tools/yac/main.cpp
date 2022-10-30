#include "yac/AST/AST.h"
#include "yac/AST/ASTVisitor.h"
#include "yac/Basic/Diagnostic.h"
#include "yac/Parse/Lexer.h"
#include "yac/Parse/Parser.h"
#include "yac/Type/Type.h"
#include <iostream>
#include <fstream>
#include <sstream>

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

  // Read source file
  std::ifstream File(inputFile);
  std::stringstream Buffer;
  Buffer << File.rdbuf();
  std::string Source = Buffer.str();

  std::cout << "\n--- Lexical Analysis ---\n";

  // Tokenize
  Lexer Lex(Source, inputFile.c_str(), Diag);
  std::vector<Token> Tokens = Lex.tokenize();

  std::cout << "Generated " << Tokens.size() << " tokens\n";

  if (Diag.hasErrors()) {
    std::cerr << "\nLexical analysis failed:\n";
    Diag.printAll(std::cerr);
    return 1;
  }

  std::cout << "\n--- Parsing ---\n";

  // Parse
  Parser Parse(std::move(Tokens), Diag, TyCtx);
  std::unique_ptr<TranslationUnit> AST = Parse.parseTranslationUnit();

  if (Diag.hasErrors()) {
    std::cerr << "\nParsing failed:\n";
    Diag.printAll(std::cerr);
    return 1;
  }

  std::cout << "✓ Parsing successful!\n";

  // Print AST if requested
  if (dumpAST || true) { // Always dump for now
    std::cout << "\n--- Abstract Syntax Tree ---\n";
    ASTPrinter Printer(std::cout);
    Printer.visitTranslationUnit(AST.get());
  }

  std::cout << "\n--- Compilation Summary ---\n";
  std::cout << "✓ Lexical analysis: OK\n";
  std::cout << "✓ Syntax analysis: OK\n";
  std::cout << "  - Declarations: " << AST->size() << "\n";

  if (Diag.getWarningCount() > 0) {
    std::cout << "⚠ Warnings: " << Diag.getWarningCount() << "\n";
    Diag.printAll(std::cout);
  }

  std::cout << "\nNext steps:\n";
  std::cout << "  1. Semantic analysis (type checking)\n";
  std::cout << "  2. Code generation (LLVM or 8086)\n";
  std::cout << "  3. Optimization passes\n";

  return 0;
}
