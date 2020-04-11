#include "xyxy/compiler.h"

#include "gtest/gtest.h"
#include "xyxy/vm.h"

namespace xyxy {

static Token CreateToken(TokenType type, int start, int leng, int line) {
  return Token{type, start, leng, line};
}

// class TestCompiler : public ::testing::Test {
//  protected:
//   void SetUp() override {
//     source_ = R"(
//       ! (5 - 4 > 3 * 2 == ! nil)
//     )";
//     chunk_ = std::make_shared<Chunk>();
//     parser_ = std::make_shared<Compiler>(source_, chunk_);
//   }

//   string source_;
//   std::shared_ptr<Compiler> parser_;
//   std::shared_ptr<Chunk> chunk_;
// };

// TEST_F(TestCompiler, Basic) {
//   // Advance one token.
//   parser_->Advance();
//   EXPECT_EQ(parser_->PrevToken(), CreateToken(TOKEN_NONE, 0, 0, 0));
//   EXPECT_EQ(parser_->CurrToken(), CreateToken(TOKEN_BANG, 0, 1, 1));

//   // Advance one token.
//   parser_->Advance();
//   EXPECT_EQ(parser_->PrevToken(), CreateToken(TOKEN_BANG, 0, 1, 1));
//   EXPECT_EQ(parser_->CurrToken(), CreateToken(TOKEN_LEFT_PAREN, 2, 1, 1));

//   // Test lexeme.
//   EXPECT_EQ(parser_->GetLexeme(parser_->CurrToken()), "(");

//   // TODO(zq7): add consume test with error handling.
// }

// TEST_F(TestCompiler, Emit) {
//   parser_->EmitByte(0);
//   parser_->EmitByte(1, 2);
//   EXPECT_EQ(chunk_->GetByte(0), 0);
//   EXPECT_EQ(chunk_->GetByte(1), 1);
//   EXPECT_EQ(chunk_->GetByte(2), 2);
//   parser_->EmitReturn();
//   EXPECT_EQ(chunk_->GetByte(3), uint8(OP_RETURN));
//   // The first value has index 0.
//   EXPECT_EQ(parser_->MakeConstant(Value(1.23)), 0);
//   EXPECT_EQ(parser_->MakeConstant(Value(false)), 1);
//   EXPECT_EQ(parser_->MakeConstant(XYXY_NIL), 2);
//   // Test emit a constant.
//   parser_->EmitConstant(Value(4.56));
//   EXPECT_EQ(chunk_->GetByte(4), uint8(OP_CONSTANT));
//   // Since EmitConstant will add one value into chunk.
//   // here the index should be 4.
//   EXPECT_EQ(parser_->MakeConstant(XYXY_NIL), 4);
// }

TEST(TestSimple, Basic) {
  Compiler compiler = Compiler();
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
}

TEST(Print, TestCompiler) {
  Compiler compiler = Compiler();
  compiler.Compile("print 1 + 2;");
  compiler.Compile("print 1 + 3;");
  compiler.Compile("print 1 + 2 * 10 - (2 + 3) * 6;");
  VM vm(compiler.GetChunk());
  EXPECT_EQ(vm.DumpInsts(), R"(
    OP_CONSTANT
  )");
  vm.Run();
}

TEST(GlobalVariable, TestCompiler) {
  Compiler compiler = Compiler();
  string source = R"(
    var xy = "aaaaaaaaa";
    print xy;
  )";
  compiler.Compile(source);
  VM vm(compiler.GetChunk());
  vm.Run();
  EXPECT_TRUE(vm.GetStack().Empty());
}

TEST(StringAdd, TestCompiler) {
  Compiler compiler = Compiler();
  string source = R"(
    var xy = "aaaaaaaaa";
    var xx = xy + "bbbbbb";
    print xy;
    print xx;
  )";
  compiler.Compile(source);
  VM vm(compiler.GetChunk());
  vm.Run();
  EXPECT_TRUE(vm.GetStack().Empty());
}

}  // namespace xyxy
