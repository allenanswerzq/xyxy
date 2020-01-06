#include "parser.h"

#include "debug.h"
#include "inst.h"
#include "vm.h"
#include "gtest/gtest.h"

namespace qian {

static Token create_token(TokenType type, int start, int leng, int line) {
  return Token{type, start, leng, line};
}

class TestParser : public ::testing::Test {
 protected:
  void SetUp() override {
    source_ = R"(
      ! (5 - 4 > 3 * 2 == ! nil)
    )";
    parser_ = std::make_shared<Parser>(source_, &chunk_);
  }

  string source_;
  std::shared_ptr<Parser> parser_;
  Chunk chunk_;
};

TEST_F(TestParser, Basic) {
  EXPECT_EQ(parser_->prev_token(), create_token(TOKEN_NONE, -1, -1, -1));
  EXPECT_EQ(parser_->curr_token(), create_token(TOKEN_NONE, -1, -1, -1));

  // Advance one token.
  parser_->advance();
  EXPECT_EQ(parser_->prev_token(), create_token(TOKEN_NONE, -1, -1, -1));
  EXPECT_EQ(parser_->curr_token(), create_token(TOKEN_BANG, 0, 1, 1));

  // Advance one token.
  parser_->advance();
  EXPECT_EQ(parser_->prev_token(), create_token(TOKEN_BANG, 0, 1, 1));
  EXPECT_EQ(parser_->curr_token(), create_token(TOKEN_LEFT_PAREN, 2, 1, 1));

  // Test lexeme.
  EXPECT_EQ(parser_->to_string(parser_->curr_token()), "(");

  // TODO(zq7): add consume test with error handling.
}

TEST_F(TestParser, Emit) {
  parser_->emit_byte(0);
  parser_->emit_byte(1, 2);
  EXPECT_EQ(chunk_.GetByte(0), 0);
  EXPECT_EQ(chunk_.GetByte(1), 1);
  EXPECT_EQ(chunk_.GetByte(2), 2);
  parser_->emit_return();
  EXPECT_EQ(chunk_.GetByte(3), uint8(OP_RETURN));
  // The first value has index 0.
  EXPECT_EQ(parser_->make_constant(QIAN_NUMBER(1.23)), 0);
  EXPECT_EQ(parser_->make_constant(QIAN_BOOL(false)), 1);
  EXPECT_EQ(parser_->make_constant(QIAN_NIL), 2);
  // Test emit a constant.
  parser_->emit_constant(QIAN_NUMBER(4.56));
  EXPECT_EQ(chunk_.GetByte(4), uint8(OP_CONSTANT));
  // Since emit_constant will add one value into chunk.
  // here the index should be 4.
  EXPECT_EQ(parser_->make_constant(QIAN_NIL), 4);
}

TEST_F(TestParser, Parse) {
  EXPECT_TRUE(parser_->get_rule(TOKEN_FALSE)->prefix_rule);
  EXPECT_TRUE(parser_->get_rule(TOKEN_TRUE)->prefix_rule);
  auto rule = parser_->get_rule(TOKEN_NIL);
  EXPECT_TRUE(rule);
  EXPECT_TRUE(rule->prefix_rule);
  parser_->advance();
  parser_->parse_expression();
  // VM vm(&chunk_);
  // vm.Run();
}

} // namespace qian
