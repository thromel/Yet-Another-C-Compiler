#ifndef YAC_BASIC_SOURCELOCATION_H
#define YAC_BASIC_SOURCELOCATION_H

#include <cstdint>
#include <string>

namespace yac {

/// Represents a location in source code
class SourceLocation {
  uint32_t Line = 0;
  uint32_t Column = 0;
  const char* Filename = nullptr;

public:
  SourceLocation() = default;
  SourceLocation(uint32_t Line, uint32_t Column, const char* Filename = nullptr)
      : Line(Line), Column(Column), Filename(Filename) {}

  uint32_t getLine() const { return Line; }
  uint32_t getColumn() const { return Column; }
  const char* getFilename() const { return Filename; }

  bool isValid() const { return Line != 0; }

  std::string toString() const {
    if (!isValid()) return "<invalid>";
    std::string result;
    if (Filename) {
      result += Filename;
      result += ":";
    }
    result += std::to_string(Line);
    result += ":";
    result += std::to_string(Column);
    return result;
  }
};

/// Represents a range in source code
class SourceRange {
  SourceLocation Begin;
  SourceLocation End;

public:
  SourceRange() = default;
  SourceRange(SourceLocation Begin, SourceLocation End)
      : Begin(Begin), End(End) {}
  SourceRange(SourceLocation Loc) : Begin(Loc), End(Loc) {}

  SourceLocation getBegin() const { return Begin; }
  SourceLocation getEnd() const { return End; }

  bool isValid() const { return Begin.isValid(); }
};

} // namespace yac

#endif // YAC_BASIC_SOURCELOCATION_H
