#include "xyxy/compiler.h"

#include "gtest/gtest.h"
#include "xyxy/vm.h"

namespace xyxy {

static Token CreateToken(TokenType type, int start, int len, int line) {
  return Token{type, start, len, line};
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
  EXPECT_EQ(vm.FinalResult(), "-9.000000");
  EXPECT_TRUE(vm.GetStack().Empty());
}

// TOOD(): OP_GET_GLOBAL will leave a value onto the stack, figure out
// how to deal with that.
#define XY_COMPILE_AND_RUN(source, result) \
  Compiler compiler;                       \
  compiler.Compile(source);                \
  VM vm(compiler.GetChunk());              \
  vm.Run();                                \
  EXPECT_EQ(vm.FinalResult(), result);     \
  if (!vm.GetStack().Empty()) {            \
    vm.DumpStack();                        \
  }                                        \
  EXPECT_TRUE(vm.GetStack().Empty());

TEST(SingleStmt, TestCompiler) {
  XY_COMPILE_AND_RUN("print 1 + 2;", "3.000000")
}

TEST(CompileMultipleStmts, TestCompiler) {
  Compiler compiler;
  compiler.Compile("print 1 + 2;");
  compiler.Compile("print 1 + 3;");
  compiler.Compile("print 1 + 2 * 10 - (2 + 3) * 6;");
  VM vm(compiler.GetChunk());
  vm.Run();
  EXPECT_EQ(vm.FinalResult(), "-9.000000");
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
  // Test single false branch.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (false) {
      a = 2;
    }
    print a;
  )",
                     "1.000000")
}

TEST(IfElse1, TestCompiler) {
  // Test single true branch
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (true) {
      a = 2;
    }
    print a;
  )",
                     "2.000000")
}

TEST(IfElse2, TestCompiler) {
  // Test flase and true together
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (false) {
      a = 2;
    }
    else {
      a = 3;
    }
    print a;
  )",
                     "3.000000")
}

TEST(IfElse3, TestCompiler) {
  // Test flase and true together.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (true) {
      a = 2;
    }
    else {
      a = 3;
    }
    print a;
  )",
                     "2.000000")
}

TEST(IfElse4, TestCompiler) {
  // Test use None as condition.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (!nil) {
      a = 2;
    }
    else {
      a = 3;
    }
    print a;
  )",
                     "2.000000")
}

TEST(IfElse5, TestCompiler) {
  // Test use an expression as condition.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (2 > 1) {
      a = 2;
    }
    print a;
  )",
                     "2.000000")
}

TEST(IfElse6, TestCompiler) {
  // Test use a assignment as condition.
  XY_COMPILE_AND_RUN(R"(
    var a = 1;
    if (a == 1) {
      a = 2;
    }
    print a;
  )",
                     "2.000000")
}

TEST(IfElse7, TestCompiler) {
  // Test use an expression as condition.
  XY_COMPILE_AND_RUN(R"(
    var a = 3;
    if (a == 1) {
      a = 2;
    }
    elif (a == 2) {
      a = 3;
    }
    else if (a == 3) {
      a = 5;
    }
    else {
      a = 4;
    }
    print a;
  )",
                     "5.000000")
}

TEST(LogicAnd, TestCompiler) {
  // Test logic and operation.
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    if (a > 1 and a > 4) {
      a = 8;
    }
    if (a > 1 and a >= 2) {
      a = a + 1;
    }
    print a;
  )",
                     "3.000000")
}

TEST(LogicAndThree, TestCompiler) {
  // Test logic and operation.
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    if (a > 1 and a > 4 and false) {
      // not get in
      a = 8;
    }
    print a;
  )",
                     "2.000000")
}

TEST(LogicOr, TestCompiler) {
  // Test logic or operation.
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    if (a > 1 or a > 4) {
      // get in
      a = 8;
    }
    if (a > 9 or a > 10) {
      // not get in
      a = a + 1;
    }
    print a;
  )",
                     "8.000000")
}

TEST(IfElse8, TestCompiler) {
  // Test nesting if else statements.
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    if (true) {
      if (true) {
        var b = 3;
        if (false) {
        }
        elif (false) {
        }
        else {
          a = a + b;
        }
      }
      if (true) {
        a = a + 1;
      }
    }
    print a;
  )",
                     "6.000000")
}

TEST(WhileStmt, TestCompiler) {
  // Test while statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    while (a < 10) {
      a = a + 1;
    }
    print a;
  )",
                     "10.000000");
}

TEST(WhileFalseStmt, TestCompiler) {
  // Test while statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 10;
    while (a > 100) {
      a = a + 1;
    }
    print a;
  )",
                     "10.000000");
}

TEST(ForStmt, TestCompiler) {
  // Test for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 10; i = i + 1) {
      a = a + 1;
    }
    print a;
  )",
                     "10.000000");
}

TEST(MultipleForStmt, TestCompiler) {
  // Test 2 for statements.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 10; i = i + 1) {
      for (var j = 0; j < 10; j = j + 1) {
        a = a + 1;
      }
    }
    print a;
  )",
                     "100.000000");
}

TEST(MultipleForStmt1, TestCompiler) {
  // Test 3 for statements.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 10; i = i + 1) {
      for (var j = 0; j < 10; j = j + 1) {
        for (var k = 0; k < 10; k = k + 1) {
          a = a + 1;
        }
      }
    }
    print a;
  )",
                     "1000.000000");
}

TEST(ForStmt1, TestCompiler) {
  // Test for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 10; ) {
      i = i + 1;
      a = a + 1;
    }
    print a;
  )",
                     "10.000000");
}

TEST(ForStmt2, TestCompiler) {
  // Test for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    var i = 0;
    for (; i < 10; ) {
      i = i + 1;
      a = a + 1;
    }
    print a;
  )",
                     "10.000000");
}

TEST(ForBreak0, TestCompiler) {
  // Test break in a for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (;;) {
      break;
    }
    print a;
  )",
                     "0.000000");
}

TEST(ForBreak1, TestCompiler) {
  // Test break in a for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 2; i < 10; i = i + 1) {
      var b;
      var c;
      var d;
      a = a + 1;
      if (a > 0) {
        break;
      }
      var e;
      var f;
      var g;
      a = a + 1;
    }
    print a;
  )",
                     "1.000000");
}

TEST(ForBreak2, TestCompiler) {
  // Test break in a for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 100; i = i + 1) {
      a = a + 1;
      if (a > 10) {
        break;
      }
    }
    print a;
  )",
                     "11.000000");
}

TEST(ForBreak3, TestCompiler) {
  // Test remove all local variables after breaking
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    for (var i = 0; i < 1; i = i + 1) {
      {
        a = a + 1;
        var b;
        var c;
        if (a > 0) {
          break;
        }
        var d;
        var e;
        a = a + 1;
      }
    }
    print a;
  )",
                     "3.000000");
}

TEST(ForBreak4, TestCompiler) {
  // Test remove all local variables after breaking
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    for (var i = 0; i < 10; i = i + 1) {
      {
        a = a + 1;
        var b = "b";
        var c = "c";
        if (a > 10) {
          // should only pop var b and c out from stack.
          break;
        }
        var d;
        var e;
        if (a > 0) {
          break;
        }
        a = a + 1;
      }
    }
    print a;
  )",
                     "3.000000");
}

TEST(ForBreak5, TestCompiler) {
  // Test remove all local variables after breaking
  XY_COMPILE_AND_RUN(R"(
    var a = 2;
    for (var i = 0; i < 100; i = i + 1) {
      a = a + 1;
      var b = "b";
      var c = "c";
      if (a > 10) {
        // Exit place.
        break;
      }
      for (var j = 2; j < 20; j = j + 1) {
        var e = "e";
        var f = "f";
        var g = "g";
        var h = "h";
        a = a + 1;
        if (a > 0) {
          // First break from here
          break;
        }
      }
    }
    print a;
  )",
                     "11.000000");
}

TEST(ForBreak6, TestCompiler) {
  // Test remove all local variables after breaking
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 10; i = i + 1) {
      {
          a = a + 1;
          var b;
          var c;
          for (var j = 0; j < 2; j = j + 1) {
            var e;
            var f;
            var g;
            var h;
            a = a + 1;
          }
          if (a > 0) {
            // Break from here
            break;
          }
          var e;
          var f;
          var g;
          var h;
      }
    }
    print a;
  )",
                     "3.000000");
}

TEST(ForContinue, TestCompiler) {
  // Test continue inside a for statement.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 1; i = i + 1) {
      continue;
      a = a + 1;
    }
    print a;
  )",
                     "0.000000");
}

TEST(ForContinue1, TestCompiler) {
  // Test remove all local variables.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 1; i = i + 1) {
      var b = "b"; // should be poped.
      var c = "c"; // should be poped.
      var d = "d"; // should be poped.
      continue;
      a = a + 1;
      var e = "e"; // should do nothing.
    }
    print a;
  )",
                     "0.000000");
}

TEST(ForContinue2, TestCompiler) {
  // Test nesting use continue.
  XY_COMPILE_AND_RUN(R"(
    var a = 0;
    for (var i = 0; i < 2; i = i + 1) {
      for (var j = 0; j < 3; j = j + 1) {
        if (j > 1) {
          continue;
        }
        a = a + 1;
      }
      if (i > 0) {
        continue;
      }
      a = a + 1;
    }
    print a;
  )",
                     "5.000000");
}

}  // namespace xyxy
