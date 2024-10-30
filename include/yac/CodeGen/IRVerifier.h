#ifndef YAC_CODEGEN_IRVERIFIER_H
#define YAC_CODEGEN_IRVERIFIER_H

#include "yac/CodeGen/IR.h"
#include <string>
#include <vector>

namespace yac {

/// IRVerifier - checks IR invariants and reports errors
class IRVerifier {
public:
  struct Error {
    std::string Message;
    IRFunction* Function = nullptr;
    IRBasicBlock* Block = nullptr;
    IRInstruction* Instruction = nullptr;
  };

private:
  std::vector<Error> Errors;
  bool FailFast = false;

public:
  IRVerifier(bool FailFast = false) : FailFast(FailFast) {}

  /// Verify entire module
  bool verify(IRModule* M);

  /// Verify single function
  bool verifyFunction(IRFunction* F);

  /// Verify single basic block
  bool verifyBasicBlock(IRBasicBlock* BB);

  /// Get all errors
  const std::vector<Error>& getErrors() const { return Errors; }

  /// Print errors to stderr
  void printErrors() const;

private:
  void addError(const std::string& Msg, IRFunction* F = nullptr,
                IRBasicBlock* BB = nullptr, IRInstruction* I = nullptr);

  // Specific invariant checks
  bool checkBlockTerminator(IRBasicBlock* BB);
  bool checkCFGConsistency(IRBasicBlock* BB);
  bool checkPhiNodes(IRBasicBlock* BB);
  bool checkDefBeforeUse(IRFunction* F);
  bool checkNoOrphanedBlocks(IRFunction* F);
};

} // namespace yac

#endif // YAC_CODEGEN_IRVERIFIER_H
