#include "scanner.h"

#include "debug.h"
#include "gtest/gtest.h"

namespace qian {

TEST(Basic, TestScanner) {
  string source = Strip(R"(

    var b = "qian";

  )");
  EXPECT_EQ(source, "var b = \"qian\";");

  Scanner sc(source);
  EXPECT_EQ(sc.at_end(), false);
  EXPECT_EQ(sc.peek(), 'v');
  EXPECT_EQ(sc.peek_next(), 'a');
  // Advance one step.
  EXPECT_EQ(sc.advance(), 'v');
  EXPECT_EQ(sc.peek(), 'a');
  EXPECT_EQ(sc.peek_next(), 'r');
  // Advance one step.
  EXPECT_EQ(sc.advance(), 'a');
  EXPECT_EQ(sc.advance(), 'r');
  EXPECT_EQ(sc.get_lexeme(), "var");
  // Space.
  EXPECT_EQ(sc.advance(), ' ');
  // Match and implicitly advance one step.
  EXPECT_EQ(sc.peek(), 'b');
  EXPECT_EQ(sc.peek_next(), ' ');
  EXPECT_TRUE(sc.match('b'));
  EXPECT_EQ(sc.peek(), ' ');
}

static bool CompareToken(Scanner& sc, string lexeme, const Token& b) {
  Token a = sc.ScanToken();
  bool r = a.type == b.type;
  r &= a.start == b.start;
  r &= a.length == b.length;
  r &= a.line == b.line;
  r &= sc.get_lexeme() == lexeme;
  return r;
}

TEST(Token, TestScanner) {
  string source = Strip(R"(
    var b = "qian";
  )");
  Scanner sc(source);
  EXPECT_TRUE(CompareToken(sc, "var",      Token{TOKEN_VAR, 0, 3, 1}));
  EXPECT_TRUE(CompareToken(sc, "b",        Token{TOKEN_IDENTIFIER, 4, 1, 1}));
  EXPECT_TRUE(CompareToken(sc, "=",        Token{TOKEN_EQUAL, 6, 1, 1}));
  EXPECT_TRUE(CompareToken(sc, "\"qian\"", Token{TOKEN_STRING, 8, 6, 1}));
  EXPECT_TRUE(CompareToken(sc, ";",        Token{TOKEN_SEMICOLON, 14, 1, 1}));
  EXPECT_TRUE(sc.at_end());
}

TEST(Number, TestScanner) {
  string source = Strip(R"(
    var x = 1.2346;
  )");
  Scanner sc(source);
  EXPECT_TRUE(CompareToken(sc, "var",      Token{TOKEN_VAR, 0, 3, 1}));
  EXPECT_TRUE(CompareToken(sc, "x",        Token{TOKEN_IDENTIFIER, 4, 1, 1}));
  EXPECT_TRUE(CompareToken(sc, "=",        Token{TOKEN_EQUAL, 6, 1, 1}));
  EXPECT_TRUE(CompareToken(sc, "1.2346",   Token{TOKEN_NUMBER, 8, 6, 1}));
  EXPECT_TRUE(CompareToken(sc, ";",        Token{TOKEN_SEMICOLON, 14, 1, 1}));
  EXPECT_TRUE(sc.at_end());
}

TEST(Nil, TestScanner) {
  string source = Strip(R"(
    var x = nil;
  )");
  Scanner sc(source);
  EXPECT_TRUE(CompareToken(sc, "var",      Token{TOKEN_VAR, 0, 3, 1}));
  EXPECT_TRUE(CompareToken(sc, "x",        Token{TOKEN_IDENTIFIER, 4, 1, 1}));
  EXPECT_TRUE(CompareToken(sc, "=",        Token{TOKEN_EQUAL, 6, 1, 1}));
  EXPECT_TRUE(CompareToken(sc, "nil",      Token{TOKEN_NIL, 8, 3, 1}));
  EXPECT_TRUE(CompareToken(sc, ";",        Token{TOKEN_SEMICOLON, 11, 1, 1}));
  EXPECT_TRUE(sc.at_end());
}

}  // namespace qian
