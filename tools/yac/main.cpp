#include "yac/AST/AST.h"
#include "yac/AST/ASTVisitor.h"
#include "yac/Basic/Diagnostic.h"
#include "yac/CodeGen/IR.h"
#include "yac/CodeGen/IRBuilder.h"
#include "yac/CodeGen/IRVerifier.h"
#include "yac/CodeGen/Pass.h"
#include "yac/CodeGen/Transforms.h"
#include "yac/CodeGen/X86_64Backend.h"
#include "yac/Parse/Lexer.h"
#include "yac/Parse/Parser.h"
#include "yac/Sema/Sema.h"
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
            << "  -S                   Emit assembly (output to <file>.s)\n"
            << "  -c                   Compile to object file\n"
            << "  -emit-ir             Emit optimized IR (output to <file>.ir)\n"
            << "  -emit-asm            Same as -S\n"
            << "  --dump-ast           Dump AST to stdout\n"
            << "  --dump-tokens        Dump tokens\n"
            << "  --dump-ir            Dump IR to stdout\n"
            << "  --dump-cfg           Dump CFG (control flow graph)\n"
            << "  --verify             Verify IR after generation\n"
            << "  --verify-each        Verify IR after each pass\n"
            << "  -fsyntax-only        Check syntax only\n"
            << "  -ftime-report        Report per-pass timing statistics\n"
            << "  -O<level>            Optimization level (0-3, default: 0)\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printUsage(argv[0]);
    return 1;
  }

  // Parse command line arguments
  std::string inputFile;
  std::string outputFile;
  bool dumpAST = false;
  bool dumpIR = false;
  bool dumpCFG = false;
  bool syntaxOnly = false;
  bool verifyIR = false;
  bool verifyEach = false;
  bool timeReport = false;
  bool emitIR = false;
  bool emitAsm = false;
  int optLevel = 0;
  (void)dumpAST;                  // Suppress unused warning for now
  (void)syntaxOnly;               // Suppress unused warning for now

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      printUsage(argv[0]);
      return 0;
    } else if (arg == "--dump-ast") {
      dumpAST = true;
    } else if (arg == "--dump-ir") {
      dumpIR = true;
    } else if (arg == "--dump-cfg") {
      dumpCFG = true;
    } else if (arg == "--verify") {
      verifyIR = true;
    } else if (arg == "--verify-each") {
      verifyEach = true;
      verifyIR = true;  // Implies --verify
    } else if (arg == "-fsyntax-only") {
      syntaxOnly = true;
    } else if (arg == "-ftime-report") {
      timeReport = true;
    } else if (arg == "-emit-ir") {
      emitIR = true;
    } else if (arg == "-S" || arg == "-emit-asm") {
      emitAsm = true;
    } else if (arg == "-o" && i + 1 < argc) {
      outputFile = argv[++i];
    } else if (arg.substr(0, 2) == "-O" && arg.length() == 3) {
      optLevel = arg[2] - '0';
      if (optLevel < 0 || optLevel > 3) {
        std::cerr << "Invalid optimization level: " << arg << "\n";
        return 1;
      }
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

  std::cout << "\n--- Semantic Analysis ---\n";

  // Semantic analysis (type checking, symbol resolution)
  Sema SemanticAnalyzer(Diag, TyCtx);
  SemanticAnalyzer.analyze(AST.get());

  if (Diag.hasErrors()) {
    std::cerr << "\nSemantic analysis failed:\n";
    Diag.printAll(std::cerr);
    return 1;
  }

  std::cout << "✓ Semantic analysis successful!\n";

  std::cout << "\n--- IR Generation ---\n";

  // Generate IR
  IRBuilder Builder(TyCtx);
  std::unique_ptr<IRModule> IR = Builder.generateIR(AST.get());

  std::cout << "✓ IR generation successful!\n";

  // Verify IR if requested
  if (verifyIR && optLevel == 0) {
    std::cout << "\n--- IR Verification ---\n";
    IRVerifier Verifier(false); // Don't fail fast
    if (Verifier.verify(IR.get())) {
      std::cout << "✓ IR verification passed!\n";
    } else {
      std::cerr << "\n✗ IR verification failed:\n";
      Verifier.printErrors();
      return 1;
    }
  }

  // Run optimization passes
  if (optLevel > 0) {
    std::cout << "\n--- Optimization (O" << optLevel << ") ---\n";

    PassManager PM(verifyEach);
    PM.setEnableTiming(timeReport);

    // Configure passes based on optimization level
    if (optLevel >= 1) {
      // -O1: Basic optimizations
      PM.addPass(std::make_unique<SimplifyCFGPass>());
      PM.addPass(std::make_unique<Mem2RegPass>());
      PM.addPass(std::make_unique<CopyPropagationPass>());
      PM.addPass(std::make_unique<ConstantPropagationPass>());
      PM.addPass(std::make_unique<DCEPass>());
    }

    if (optLevel >= 2) {
      // -O2: More aggressive optimizations with advanced passes
      PM.addPass(std::make_unique<SimplifyCFGPass>());
      PM.addPass(std::make_unique<SCCPPass>());           // Sparse conditional constant propagation
      PM.addPass(std::make_unique<GVNPass>());            // Global value numbering (CSE)
      PM.addPass(std::make_unique<CopyPropagationPass>());
      PM.addPass(std::make_unique<DCEPass>());
      PM.addPass(std::make_unique<LICMPass>());           // Loop invariant code motion
      PM.addPass(std::make_unique<SimplifyCFGPass>());    // Cleanup after LICM
    }

    if (optLevel >= 3) {
      // -O3: Maximum optimizations (additional rounds)
      PM.addPass(std::make_unique<SCCPPass>());
      PM.addPass(std::make_unique<GVNPass>());
      PM.addPass(std::make_unique<CopyPropagationPass>());
      PM.addPass(std::make_unique<DCEPass>());
      PM.addPass(std::make_unique<LICMPass>());
      PM.addPass(std::make_unique<SimplifyCFGPass>());
    }

    // Run passes
    bool Changed = PM.run(IR.get());
    if (Changed) {
      std::cout << "✓ Optimizations applied\n";
    } else {
      std::cout << "  No changes made\n";
    }

    // Print timing report if requested
    if (timeReport) {
      PM.printTimingReport();
    }

    // Verify after optimization
    if (verifyIR) {
      std::cout << "\n--- IR Verification (post-optimization) ---\n";
      IRVerifier Verifier(false);
      if (Verifier.verify(IR.get())) {
        std::cout << "✓ IR verification passed!\n";
      } else {
        std::cerr << "\n✗ IR verification failed:\n";
        Verifier.printErrors();
        return 1;
      }
    }
  }

  // Dump IR if requested
  if (dumpIR) {
    std::cout << "\n--- IR Dump ---\n";
    IR->print();
  }

  // Dump CFG if requested
  if (dumpCFG) {
    std::cout << "\n--- Control Flow Graph ---\n";
    for (const auto& F : IR->getFunctions()) {
      std::cout << "\nFunction: " << F->getName() << "\n";
      for (const auto& BB : F->getBlocks()) {
        std::cout << "  Block: " << BB->getName() << "\n";
        std::cout << "    Predecessors: ";
        for (const auto* Pred : BB->getPredecessors()) {
          std::cout << Pred->getName() << " ";
        }
        std::cout << "\n";
        std::cout << "    Successors: ";
        for (const auto* Succ : BB->getSuccessors()) {
          std::cout << Succ->getName() << " ";
        }
        std::cout << "\n";
      }
    }
  }

  // Print IR (always shown unless --dump-ir is used)
  if (!dumpIR) {
    std::cout << "\n--- Intermediate Representation ---\n";
    IR->print();
  }

  // Print AST if requested
  if (dumpAST) {
    std::cout << "\n--- Abstract Syntax Tree ---\n";
    ASTPrinter Printer(std::cout);
    Printer.visitTranslationUnit(AST.get());
  }

  // Emit IR to file if requested
  if (emitIR) {
    std::string irFile = outputFile.empty() ? (inputFile + ".ir") : outputFile;
    std::ofstream irOut(irFile);
    if (!irOut.good()) {
      std::cerr << "Error: Cannot write to file: " << irFile << "\n";
      return 1;
    }

    // Redirect IR print to file
    std::streambuf* oldCoutBuf = std::cout.rdbuf();
    std::cout.rdbuf(irOut.rdbuf());

    std::cout << "=== YAC Intermediate Representation ===\n";
    std::cout << "Source: " << inputFile << "\n";
    std::cout << "Optimization Level: -O" << optLevel << "\n\n";
    IR->print();

    std::cout.rdbuf(oldCoutBuf);
    irOut.close();

    std::cout << "✓ IR written to: " << irFile << "\n";
  }

  // Emit assembly to file if requested
  if (emitAsm) {
    std::string asmFile = outputFile.empty() ? (inputFile + ".s") : outputFile;
    std::ofstream asmOut(asmFile);
    if (!asmOut.good()) {
      std::cerr << "Error: Cannot write to file: " << asmFile << "\n";
      return 1;
    }

    // Use the x86-64 backend to generate assembly
    asmOut << "# Assembly output for: " << inputFile << "\n";
    asmOut << "# Generated by YAC Compiler v0.5.0\n";
    asmOut << "# Optimization level: -O" << optLevel << "\n\n";

    X86_64Backend Backend(asmOut);
    Backend.generateAssembly(IR.get());

    asmOut.close();

    std::cout << "✓ Assembly written to: " << asmFile << "\n";
  }

  std::cout << "\n--- Compilation Summary ---\n";
  std::cout << "✓ Lexical analysis: OK\n";
  std::cout << "✓ Syntax analysis: OK\n";
  std::cout << "✓ Semantic analysis: OK\n";
  std::cout << "✓ IR generation: OK\n";
  std::cout << "  - Declarations: " << AST->size() << "\n";
  std::cout << "  - Functions: " << IR->getFunctions().size() << "\n";

  if (Diag.getWarningCount() > 0) {
    std::cout << "⚠ Warnings: " << Diag.getWarningCount() << "\n";
    Diag.printAll(std::cout);
  }

  std::cout << "\nNext steps:\n";
  std::cout << "  1. Assembly code generation\n";
  std::cout << "  2. Optimization passes\n";

  return 0;
}
