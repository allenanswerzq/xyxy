#include "xyxy/compiler.h"

#include "gtest/gtest.h"
#include "xyxy/inst.h"
#include "xyxy/vm.h"

namespace xyxy {

static Token CreateToken(TokenType type, int start, int leng, int line) {
  return Token{type, start, leng, line};
}

class TestCompiler : public ::testing::Test {
 protected:
  void SetUp() override {
    source_ = R"(
      ! (5 - 4 > 3 * 2 == ! nil)
    )";
    chunk_ = std::make_shared<Chunk>();
    parser_ = std::make_shared<Compiler>(source_, chunk_);
  }

  string source_;
  std::shared_ptr<Compiler> parser_;
  std::shared_ptr<Chunk> chunk_;
};

TEST_F(TestCompiler, Basic) {
  // Advance one token.
  parser_->Advance();
  EXPECT_EQ(parser_->PrevToken(), CreateToken(TOKEN_NONE, 0, 0, 0));
  EXPECT_EQ(parser_->CurrToken(), CreateToken(TOKEN_BANG, 0, 1, 1));

  // Advance one token.
  parser_->Advance();
  EXPECT_EQ(parser_->PrevToken(), CreateToken(TOKEN_BANG, 0, 1, 1));
  EXPECT_EQ(parser_->CurrToken(), CreateToken(TOKEN_LEFT_PAREN, 2, 1, 1));

  // Test lexeme.
  EXPECT_EQ(parser_->GetLexeme(parser_->CurrToken()), "(");

  // TODO(zq7): add consume test with error handling.
}

TEST_F(TestCompiler, Emit) {
  parser_->EmitByte(0);
  parser_->EmitByte(1, 2);
  EXPECT_EQ(chunk_->GetByte(0), 0);
  EXPECT_EQ(chunk_->GetByte(1), 1);
  EXPECT_EQ(chunk_->GetByte(2), 2);
  parser_->EmitReturn();
  EXPECT_EQ(chunk_->GetByte(3), uint8(OP_RETURN));
  // The first value has index 0.
  EXPECT_EQ(parser_->MakeConstant(Value(1.23)), 0);
  EXPECT_EQ(parser_->MakeConstant(Value(false)), 1);
  EXPECT_EQ(parser_->MakeConstant(XYXY_NIL), 2);
  // Test emit a constant.
  parser_->EmitConstant(Value(4.56));
  EXPECT_EQ(chunk_->GetByte(4), uint8(OP_CONSTANT));
  // Since EmitConstant will add one value into chunk.
  // here the index should be 4.
  EXPECT_EQ(parser_->MakeConstant(XYXY_NIL), 4);
}

TEST_F(TestCompiler, Parse) {
  EXPECT_TRUE(parser_->GetRule(TOKEN_FALSE)->prefix_rule);
  EXPECT_TRUE(parser_->GetRule(TOKEN_TRUE)->prefix_rule);
  auto rule = parser_->GetRule(TOKEN_NIL);
  EXPECT_TRUE(rule);
  EXPECT_TRUE(rule->prefix_rule);
  parser_->ParseExpression();
  VM vm(chunk_);
  vm.Run();
}

class TestSimple : public ::testing::Test {
 protected:
  void SetUp() override {
    source_ = R"(
      1 + 2 * 10 - (2 + 3) * 6
    )";
    chunk_ = std::make_shared<Chunk>();
    parser_ = std::make_shared<Compiler>(source_, chunk_);
  }

  string source_;
  std::shared_ptr<Compiler> parser_;
  std::shared_ptr<Chunk> chunk_;
};

TEST_F(TestSimple, Basic) {
  parser_->ParseExpression();
  parser_->EmitReturn();

  EXPECT_EQ(chunk_->GetByte(0), uint8(OP_CONSTANT));
  // The index of the first const inside chunk.
  EXPECT_EQ(chunk_->GetByte(1), 0);
  EXPECT_EQ(chunk_->GetConstant(0), Value(1.0));

  EXPECT_EQ(chunk_->GetByte(2), uint8(OP_CONSTANT));
  EXPECT_EQ(chunk_->GetByte(3), 1);
  EXPECT_EQ(chunk_->GetConstant(1), Value(2.0));

  EXPECT_EQ(chunk_->GetByte(4), uint8(OP_CONSTANT));

  auto inst = DispathInst(chunk_, 0);
  auto oprd = inst->Operands();
  EXPECT_EQ(oprd.size(), 1);
  EXPECT_EQ(oprd[0], Value(1.0));

  auto inst1 = DispathInst(chunk_, 2);
  EXPECT_NE(inst, inst1);
  auto oprd1 = inst->Operands();
  EXPECT_EQ(oprd1.size(), 1);
  EXPECT_EQ(oprd1[0], Value(2.0));

  VM vm(chunk_);
  vm.Run();
}

}  // namespace xyxy
