#include "parser.h"

#include "debug.h"
#include "inst.h"
#include "vm.h"
#include "gtest/gtest.h"

namespace qian {

static bool compare(const Token& a, const Token& b) {
  bool r = a.type == b.type;
  r &= a.start == b.start;
  r &= a.length == b.length;
  r &= a.line == b.line;
  return r;
}

class TestParser : public ::testing::Test {
 protected:
  void SetUp() override {
    source_ = Strip(R"(
      ! (5 - 4 > 3 * 2 == ! nil)
    )");
    parser_ = std::make_shared<Parser>(source_, &chunk_);
  }

  string source_;
  std::shared_ptr<Parser> parser_;
  Chunk chunk_;
};

TEST_F(TestParser, Basic) {
  EXPECT_EQ(source_, "! (5 - 4 > 3 * 2 == ! nil)");
  EXPECT_TRUE(compare(parser_->prev_token(),
                      CreateToken(TOKEN_NONE, -1, -1, -1)));
  EXPECT_TRUE(compare(parser_->curr_token(),
                      CreateToken(TOKEN_NONE, -1, -1, -1)));

  // Advance one token.
  parser_->advance();
  EXPECT_TRUE(compare(parser_->prev_token(),
                      CreateToken(TOKEN_NONE, -1, -1, -1)));
  EXPECT_TRUE(compare(parser_->curr_token(),
                      CreateToken(TOKEN_BANG, 0, 1, 1)));

  // Advance one token.
  parser_->advance();
  EXPECT_TRUE(compare(parser_->prev_token(),
                      CreateToken(TOKEN_BANG, 0, 1, 1)));
  EXPECT_TRUE(compare(parser_->curr_token(),
                      CreateToken(TOKEN_LEFT_PAREN, 2, 1, 1)));

  // Test lexeme.
  EXPECT_EQ(parser_->get_lexeme(parser_->curr_token()), "(");

  // TODO(zq7): addd cosume test with error handling.
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
  parser_->advance();
  parser_->expression();
  VM vm(&chunk_);
  vm.Run();
}

} // namespace qian
