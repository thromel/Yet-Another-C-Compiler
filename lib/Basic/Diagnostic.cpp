#include "yac/Basic/Diagnostic.h"
#include <iostream>

namespace yac {

void Diagnostic::print(std::ostream& OS, bool UseColors) const {
  // Print location
  if (Loc.isValid()) {
    OS << Loc.toString() << ": ";
  }

  // Print kind with optional colors
  if (UseColors) {
    switch (Kind) {
    case DiagnosticKind::Error:
      OS << "\033[1;31merror:\033[0m "; // Red
      break;
    case DiagnosticKind::Warning:
      OS << "\033[1;35mwarning:\033[0m "; // Magenta
      break;
    case DiagnosticKind::Note:
      OS << "\033[1;36mnote:\033[0m "; // Cyan
      break;
    }
  } else {
    switch (Kind) {
    case DiagnosticKind::Error:
      OS << "error: ";
      break;
    case DiagnosticKind::Warning:
      OS << "warning: ";
      break;
    case DiagnosticKind::Note:
      OS << "note: ";
      break;
    }
  }

  // Print message
  OS << Message << "\n";
}

void DiagnosticEngine::error(SourceLocation Loc, const std::string& Msg) {
  Diagnostics.emplace_back(DiagnosticKind::Error, Loc, Msg);
  ErrorCount++;
}

void DiagnosticEngine::warning(SourceLocation Loc, const std::string& Msg) {
  Diagnostics.emplace_back(DiagnosticKind::Warning, Loc, Msg);
  WarningCount++;
}

void DiagnosticEngine::note(SourceLocation Loc, const std::string& Msg) {
  Diagnostics.emplace_back(DiagnosticKind::Note, Loc, Msg);
}

void DiagnosticEngine::printAll(std::ostream& OS) const {
  for (const auto& Diag : Diagnostics) {
    Diag.print(OS, UseColors);
  }

  // Print summary
  if (ErrorCount > 0 || WarningCount > 0) {
    OS << "\n";
    if (ErrorCount > 0) {
      OS << ErrorCount << " error" << (ErrorCount > 1 ? "s" : "");
      if (WarningCount > 0)
        OS << " and ";
    }
    if (WarningCount > 0) {
      OS << WarningCount << " warning" << (WarningCount > 1 ? "s" : "");
    }
    OS << " generated.\n";
  }
}

} // namespace yac
