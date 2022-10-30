#ifndef YAC_BASIC_DIAGNOSTIC_H
#define YAC_BASIC_DIAGNOSTIC_H

#include "yac/Basic/SourceLocation.h"
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace yac {

enum class DiagnosticKind {
  Error,
  Warning,
  Note
};

/// Represents a single diagnostic message
class Diagnostic {
  DiagnosticKind Kind;
  SourceLocation Loc;
  std::string Message;

public:
  Diagnostic(DiagnosticKind Kind, SourceLocation Loc, std::string Message)
      : Kind(Kind), Loc(Loc), Message(std::move(Message)) {}

  DiagnosticKind getKind() const { return Kind; }
  SourceLocation getLocation() const { return Loc; }
  const std::string& getMessage() const { return Message; }

  void print(std::ostream& OS, bool UseColors = false) const;
};

/// Manages diagnostic messages
class DiagnosticEngine {
  std::vector<Diagnostic> Diagnostics;
  unsigned ErrorCount = 0;
  unsigned WarningCount = 0;
  bool UseColors = false;

public:
  DiagnosticEngine() = default;

  void setUseColors(bool UseColors) { this->UseColors = UseColors; }

  // Emit diagnostics
  void error(SourceLocation Loc, const std::string& Msg);
  void warning(SourceLocation Loc, const std::string& Msg);
  void note(SourceLocation Loc, const std::string& Msg);

  // Query state
  unsigned getErrorCount() const { return ErrorCount; }
  unsigned getWarningCount() const { return WarningCount; }
  bool hasErrors() const { return ErrorCount > 0; }

  const std::vector<Diagnostic>& getDiagnostics() const { return Diagnostics; }

  // Print all diagnostics
  void printAll(std::ostream& OS) const;

  // Clear diagnostics
  void clear() {
    Diagnostics.clear();
    ErrorCount = 0;
    WarningCount = 0;
  }
};

} // namespace yac

#endif // YAC_BASIC_DIAGNOSTIC_H
