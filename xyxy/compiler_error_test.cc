#include "xyxy/compiler.h"

// clang-format off

#include "gtest/gtest.h"
#include "xyxy/vm.h"

// clang-format on

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

}  // namespace xyxy
