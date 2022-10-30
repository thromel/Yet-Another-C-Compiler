#include "yac/Basic/Diagnostic.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace yac;

class DiagnosticTest : public ::testing::Test {
protected:
  DiagnosticEngine Diag;
  SourceLocation Loc{10, 5, "test.c"};
};

TEST_F(DiagnosticTest, ErrorCount) {
  EXPECT_EQ(Diag.getErrorCount(), 0);
  EXPECT_FALSE(Diag.hasErrors());

  Diag.error(Loc, "Test error");

  EXPECT_EQ(Diag.getErrorCount(), 1);
  EXPECT_TRUE(Diag.hasErrors());
}

TEST_F(DiagnosticTest, WarningCount) {
  EXPECT_EQ(Diag.getWarningCount(), 0);

  Diag.warning(Loc, "Test warning");
  Diag.warning(Loc, "Another warning");

  EXPECT_EQ(Diag.getWarningCount(), 2);
}

TEST_F(DiagnosticTest, MixedDiagnostics) {
  Diag.error(Loc, "Error 1");
  Diag.warning(Loc, "Warning 1");
  Diag.error(Loc, "Error 2");
  Diag.note(Loc, "Note 1");

  EXPECT_EQ(Diag.getErrorCount(), 2);
  EXPECT_EQ(Diag.getWarningCount(), 1);
  EXPECT_EQ(Diag.getDiagnostics().size(), 4);
}

TEST_F(DiagnosticTest, Clear) {
  Diag.error(Loc, "Error");
  Diag.warning(Loc, "Warning");

  EXPECT_EQ(Diag.getErrorCount(), 1);
  EXPECT_EQ(Diag.getWarningCount(), 1);

  Diag.clear();

  EXPECT_EQ(Diag.getErrorCount(), 0);
  EXPECT_EQ(Diag.getWarningCount(), 0);
  EXPECT_EQ(Diag.getDiagnostics().size(), 0);
}

TEST_F(DiagnosticTest, DiagnosticPrint) {
  Diag.error(Loc, "Something went wrong");

  std::ostringstream oss;
  const auto& diags = Diag.getDiagnostics();
  ASSERT_EQ(diags.size(), 1);

  diags[0].print(oss, false);
  std::string output = oss.str();

  EXPECT_NE(output.find("test.c:10:5"), std::string::npos);
  EXPECT_NE(output.find("error:"), std::string::npos);
  EXPECT_NE(output.find("Something went wrong"), std::string::npos);
}

TEST_F(DiagnosticTest, SourceLocation) {
  SourceLocation loc(42, 13, "example.c");

  EXPECT_EQ(loc.getLine(), 42);
  EXPECT_EQ(loc.getColumn(), 13);
  EXPECT_STREQ(loc.getFilename(), "example.c");
  EXPECT_TRUE(loc.isValid());

  std::string str = loc.toString();
  EXPECT_NE(str.find("example.c:42:13"), std::string::npos);
}
