#include "xyxy/scanner.h"

#include "gtest/gtest.h"

namespace xyxy {

TEST(Basic, TestScanner) {
  string source = R"(

    var b = a;

  )";

  Scanner sc(source);
  EXPECT_EQ(sc.AtEnd(), false);
  EXPECT_EQ(sc.CurrentPos(), 0);
  EXPECT_EQ(sc.Peek(), 'v');
  EXPECT_EQ(sc.PeekNext(), 'a');
  // Advance one step.
  EXPECT_EQ(sc.Advance(), 'v');
  EXPECT_EQ(sc.CurrentPos(), 1);
  EXPECT_EQ(sc.Peek(), 'a');
  EXPECT_EQ(sc.PeekNext(), 'r');
  // Advance one step.
  EXPECT_EQ(sc.Advance(), 'a');
  EXPECT_EQ(sc.CurrentPos(), 2);
  EXPECT_EQ(sc.Advance(), 'r');
  EXPECT_EQ(sc.GetLexeme(), "var");
  // Space.
  EXPECT_EQ(sc.Advance(), ' ');
  // Match will implicitly Advance one step.
  EXPECT_EQ(sc.Peek(), 'b');
  EXPECT_EQ(sc.PeekNext(), ' ');
  EXPECT_TRUE(sc.Match('b'));
  EXPECT_EQ(sc.Peek(), ' ');

  EXPECT_EQ(sc.Advance(), ' ');
  EXPECT_EQ(sc.Advance(), '=');
  EXPECT_EQ(sc.Advance(), ' ');
  EXPECT_EQ(sc.Advance(), 'a');
  EXPECT_EQ(sc.Advance(), ';');
  EXPECT_TRUE(sc.AtEnd());
}

static bool Compare(Scanner* sc, const string& lexeme, Token b) {
  Token a = sc->ScanToken();
  bool r = (a == b);
  // NOTE: After scanning a token, get the lexeme [start_, curr_).
  r &= sc->GetLexeme() == lexeme;
  return r;
}

TEST(Token, TestScanner) {
  string source = R"(
    var b = "xyxy";
  )";
  Scanner sc(source);
  EXPECT_TRUE(Compare(&sc, "var", Token{TOKEN_VAR, 0, 3, 1}));
  EXPECT_TRUE(Compare(&sc, "b", Token{TOKEN_IDENTIFIER, 4, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "=", Token{TOKEN_EQUAL, 6, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "\"xyxy\"", Token{TOKEN_STRING, 8, 6, 1}));
  EXPECT_TRUE(Compare(&sc, ";", Token{TOKEN_SEMICOLON, 14, 1, 1}));
  EXPECT_TRUE(sc.AtEnd());
}

TEST(Number, TestScanner) {
  string source = R"(
    var x = 1.2346;
  )";
  Scanner sc(source);
  EXPECT_TRUE(Compare(&sc, "var", Token{TOKEN_VAR, 0, 3, 1}));
  EXPECT_TRUE(Compare(&sc, "x", Token{TOKEN_IDENTIFIER, 4, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "=", Token{TOKEN_EQUAL, 6, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "1.2346", Token{TOKEN_NUMBER, 8, 6, 1}));
  EXPECT_TRUE(Compare(&sc, ";", Token{TOKEN_SEMICOLON, 14, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "", Token{TOKEN_EOF, -1, -1, -1}));
}

TEST(Nil, TestScanner) {
  string source = R"(
    var x = nil;
    print x;
  )";
  Scanner sc(source);
  EXPECT_TRUE(Compare(&sc, "var", Token{TOKEN_VAR, 0, 3, 1}));
  EXPECT_TRUE(Compare(&sc, "x", Token{TOKEN_IDENTIFIER, 4, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "=", Token{TOKEN_EQUAL, 6, 1, 1}));
  EXPECT_TRUE(Compare(&sc, "nil", Token{TOKEN_NIL, 8, 3, 1}));
  EXPECT_TRUE(Compare(&sc, ";", Token{TOKEN_SEMICOLON, 11, 1, 1}));
  // EXPECT_TRUE(sc.AtEnd());
}

TEST(TokenEqual, TestToken) {
  Token a, b;
  EXPECT_TRUE(a == b);
  // EXPECT_EQ(a, b);

  Token c{TOKEN_LESS, 0, 1, 2};
  Token d{TOKEN_LESS, 0, 1, 2};
  EXPECT_TRUE(c == d);
  // EXPECT_EQ(c, d);

  Token e{TOKEN_LESS, 0, 1, 2};
  Token f{TOKEN_EOF, 0, 1, 2};
  EXPECT_TRUE(e != f);
}

}  // namespace xyxy
