#include "xyxy/compiler.h"

#include "gtest/gtest.h"
#include "xyxy/vm.h"

namespace xyxy {

static Token CreateToken(TokenType type, int start, int leng, int line) {
  return Token{type, start, leng, line};
}

TEST(TokenParse, TestCompiler) {
  string source = "      ! (5 - 4 > 3 * 2 == ! nil) ";
  std::unique_ptr<Compiler> compiler = std::make_unique<Compiler>(source);
  auto chunk = compiler->GetChunk();

  // Advance one token.
  compiler->Advance();
  EXPECT_EQ(compiler->PrevToken(), CreateToken(TOKEN_NONE, 0, 0, 0));
  EXPECT_EQ(compiler->CurrToken(), CreateToken(TOKEN_BANG, 0, 1, 1));

  // Advance one token.
  compiler->Advance();
  EXPECT_EQ(compiler->PrevToken(), CreateToken(TOKEN_BANG, 0, 1, 1));
  EXPECT_EQ(compiler->CurrToken(), CreateToken(TOKEN_LEFT_PAREN, 2, 1, 1));

  // Test lexeme.
  EXPECT_EQ(compiler->GetLexeme(compiler->CurrToken()), "(");

  // TODO(zq7): add consume test with error handling.

  // Test emiting code.
  compiler->EmitByte(0);
  compiler->EmitByte(1, 2);
  EXPECT_EQ(chunk->GetByte(0), 0);
  EXPECT_EQ(chunk->GetByte(1), 1);
  EXPECT_EQ(chunk->GetByte(2), 2);
  compiler->EmitReturn();
  EXPECT_EQ(chunk->GetByte(3), uint8(OP_RETURN));
  // The first value has index 0.
  EXPECT_EQ(compiler->MakeConstant(Value(1.23)), 0);
  EXPECT_EQ(compiler->MakeConstant(Value(false)), 1);
  EXPECT_EQ(compiler->MakeConstant(XYXY_NIL), 2);
  // Test emit a constant.
  compiler->EmitConstant(Value(4.56));
  EXPECT_EQ(chunk->GetByte(4), uint8(OP_CONSTANT));
  // Since EmitConstant will add one value into chunk.
  // here the index should be 4.
  EXPECT_EQ(compiler->MakeConstant(XYXY_NIL), 4);
}

TEST(Basic, TestCompiler) {
  Compiler compiler;
  compiler.Compile(" print 1 + 2 * 10 - (2 + 3) * 6; ");
  auto chunk = compiler.GetChunk();

  EXPECT_EQ(chunk->GetByte(0), uint8(OP_CONSTANT));
  // The index of the first const inside chunk.
  EXPECT_EQ(chunk->GetByte(1), 0);
  EXPECT_EQ(chunk->GetConstant(0), Value(1.0));

  EXPECT_EQ(chunk->GetByte(2), uint8(OP_CONSTANT));
  EXPECT_EQ(chunk->GetByte(3), 1);
  EXPECT_EQ(chunk->GetConstant(1), Value(2.0));

  EXPECT_EQ(chunk->GetByte(4), uint8(OP_CONSTANT));

  VM vm(chunk);
  vm.Run();
  EXPECT_EQ(vm.FinalResult(), "9.000000");
  EXPECT_TRUE(vm.GetStack().Empty());
}

#define XY_COMPILE_AND_RUN(source, result) \
  Compiler compiler;                       \
  compiler.Compile(source);                \
  VM vm(compiler.GetChunk());              \
  vm.Run();                                \
  EXPECT_EQ(vm.FinalResult(), result);     \
  EXPECT_TRUE(vm.GetStack().Empty());

// TODO(): Improve this after adding error handling
#define XY_COMPILE_SHOLD_ERROR(source) \
  Compiler compiler;                   \
  compiler.Compile(source);            \
  EXPECT_DEATH({ compiler.Compile(source); }, "");

TEST(SingleStatement, TestCompiler) {
  XY_COMPILE_AND_RUN("print 1 + 2;", "3.000000")
}

TEST(CompileMultipleStatements, TestCompiler) {
  Compiler compiler;
  compiler.Compile("print 1 + 2;");
  compiler.Compile("print 1 + 3;");
  compiler.Compile("print 1 + 2 * 10 - (2 + 3) * 6;");
  VM vm(compiler.GetChunk());
  vm.Run();
  EXPECT_EQ(vm.FinalResult(), "9.000000");
  EXPECT_TRUE(vm.GetStack().Empty());
}

TEST(GlobalDef0, TestCompiler) {
  XY_COMPILE_AND_RUN(R"(
    var xy = "aaaa";
    print xy;
  )",
                     "aaaa")
}

TEST(GlobalDef1, TestCompiler) {
  // Mulitple global varibles addition.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    var b = 2;
    var c = 3;
    var d = a + b + c;
    print d;
  )",
                     "6.000000")
}

TEST(StringAdd, TestCompiler) {
  // Test two string addition.
  XY_COMPILE_AND_RUN(R"(
    var xy = "aaaa";
    var xx = xy + "bbbb";
    print xy;
    print xx;
  )",
                     "aaaabbbb")
}

TEST(GlobalSet, TestCompiler) {
  // Reassign after declaring.
  XY_COMPILE_AND_RUN(R"(
    var xy = "aaaa";
    xy = "bbbb";
    print xy;
  )",
                     "bbbb")
}

TEST(LocalDef0, TestCompiler) {
  // Normal nesting scope test.
  XY_COMPILE_AND_RUN(R"(
    {
      var a = 1;
      {
        var b = 2;
        {
          var c = 3;
          var d = a + b + c;
          print d;
        }
      }
    }
  )",
                     "6.000000")
}

TEST(LocalDef1, TestCompiler) {
  // Define variables that have same name in different scopes.
  XY_COMPILE_AND_RUN(R"(
    {
      var a = "outer";
      {
        var a = "inner";
        {
          var a = "innerest";
        }
      }
    }
  )",
                     "")
}

TEST(LocalDef2, TestCompiler) {
  // Normal nesting scope test.
  XY_COMPILE_AND_RUN(R"(
    var g = 0; {
      var a = 1; {
        var b = 2; {
          var c = 3;
          g = g + c;
        }
        g = g + b;
      }
      g = g + a;
    }
    print g;
  )",
                     "6.000000")
}

TEST(IfElse0, TestCompiler) {
  // Normal nesting scope test.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (true) {
      a = 2;
    }
    print a;
  )",
                     "1.000000")
}

}  // namespace xyxy
