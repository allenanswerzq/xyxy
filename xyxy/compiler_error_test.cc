#include "gtest/gtest.h"
#include "xyxy/compiler.h"
#include "xyxy/vm.h"

namespace xyxy {

// TODO(): Improve this after adding error handling
#define XY_COMPILE_SHOLD_ERROR(source) \
  Compiler compiler;                   \
  EXPECT_DEATH({ compiler.Compile(source); }, "");

TEST(ErrorAssignment, TestCompiler) {
  // Syntax error.
  XY_COMPILE_SHOLD_ERROR(R"(
     a * b = c + d;
  )")
}

TEST(LocalDefSame, TestCompiler) {
  // Same scope declares two variables with same name is wrong.
  XY_COMPILE_SHOLD_ERROR(R"(
    {
      var a = "first";
      var a = "second";
    }
  )")
}

TEST(LocalUnitialized, TestCompiler) {
  XY_COMPILE_SHOLD_ERROR(R"(
    {
      var a = "outer";
      {
        // Dont allow assign itself.
        var a = a;
      }
    }
  )")
}

TEST(IfElse, DISABLED_TestCompiler) {
  // TODO(): This test should fail.
  XY_COMPILE_SHOLD_ERROR(R"(
    var a = 1;
    elif (a == 1) {
      a = 2;
    }
    print a;
  )")
}

TEST(ForBreak, TestCompiler) {
  XY_COMPILE_SHOLD_ERROR(R"(
    var a = 1;
    break;
  )")
}

TEST(ForContinue, TestCompiler) {
  XY_COMPILE_SHOLD_ERROR(R"(
    var a = 1;
    continue;
  )")
}
}  // namespace xyxy
