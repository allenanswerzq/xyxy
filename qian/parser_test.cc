#include "parser.h"

#include "debug.h"
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
    parser_ = Parser(source_, &chunk_);
  }

  string source_;
  Parser parser_;
  Chunk chunk_;
};

TEST_F(TestParser, Basic) {
  EXPECT_EQ(source_, "! (5 - 4 > 3 * 2 == ! nil");
  EXPECT_TRUE(compare(parser_.prev_token(),
                      CreateToken(TOKEN_NONE, -1, -1, -1)));
  EXPECT_TRUE(compare(parser_.curr_token(),
                      CreateToken(TOKEN_NONE, -1, -1, -1)));

  // Advance one token.
  parser_.advance();
  EXPECT_TRUE(compare(parser_.prev_token(),
                      CreateToken(TOKEN_NONE, -1, -1, -1)));
  EXPECT_TRUE(compare(parser_.curr_token(),
                      CreateToken(TOKEN_BANG, 0, 1, 1)));

  // Advance one token.
  parser_.advance();
  EXPECT_TRUE(compare(parser_.prev_token(),
                      CreateToken(TOKEN_BANG, 0, 1, 1)));
  EXPECT_TRUE(compare(parser_.curr_token(),
                      CreateToken(TOKEN_LEFT_PAREN, 2, 1, 1)));
  // parser.expression();
}

} // namespace qian
