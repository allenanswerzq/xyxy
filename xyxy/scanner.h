#ifndef XYXY_SCANNER_H_
#define XYXY_SCANNER_H_

#include <glog/logging.h>

#include "xyxy/base.h"

namespace xyxy {

typedef enum {
  // Single character tokens.
  TOKEN_LEFT_PAREN,   // "("
  TOKEN_RIGHT_PAREN,  // ")"
  TOKEN_LEFT_BRACE,   // "{"
  TOKEN_RIGHT_BRACE,  // "}"
  TOKEN_COMMA,        // ","
  TOKEN_DOT,          // "."
  TOKEN_MINUS,        // "-"
  TOKEN_PLUS,         // "+"
  TOKEN_SEMICOLON,    // ";"
  TOKEN_SLASH,        // "/"
  TOKEN_STAR,         // "*"

  // Single or two character tokens.
  TOKEN_BANG,           // "!"
  TOKEN_BANG_EQUAL,     // "!="
  TOKEN_EQUAL,          // "="
  TOKEN_EQUAL_EQUAL,    // "=="
  TOKEN_GREATER,        // ">"
  TOKEN_GREATER_EQUAL,  // ">="
  TOKEN_LESS,           // "<"
  TOKEN_LESS_EQUAL,     // "<="

  // LiteralExprs
  TOKEN_IDENTIFIER,  // "identifier"
  TOKEN_STRING,      // "string"
  TOKEN_NUMBER,      // "number"

  // Keywords
  TOKEN_AND,     // "and"
  TOKEN_IF,      // "if"
  TOKEN_ELSE,    // "else"
  TOKEN_FALSE,   // "false"
  TOKEN_FUN,     // "fun"
  TOKEN_FOR,     // "for"
  TOKEN_NIL,     // "nil"
  TOKEN_OR,      // "or"
  TOKEN_CLASS,   // "class"
  TOKEN_PRINT,   // "print"
  TOKEN_RETURN,  // "return"
  TOKEN_SUPER,   // "super"
  TOKEN_THIS,    // "this"
  TOKEN_TRUE,    // "true"
  TOKEN_VAR,     // "var"
  TOKEN_WHILE,   // "while"

  TOKEN_NEWLINE,     // "\n"
  TOKEN_WHITESPACE,  // "white"

  TOKEN_ERROR,  // "error"
  TOKEN_EOF,    // "eof"

  TOKEN_NONE,  // None token
} TokenType;

struct Token {
  TokenType type;
  int start;
  int length;
  int line;
  friend bool operator==(const Token& a, const Token& b);
  friend bool operator!=(const Token& a, const Token& b);
};

string TokeTypeName();

class Scanner {
 public:
  Scanner(const string& source) {
    source_ = StripSource(source);
    start_ = 0;
    current_ = 0;
    line_ = 1;
  }

  Token ScanToken();

  string GetSource(int start, int end);

  string StripSource(const string& str);

  bool AtEnd() { return current_ == source_.size(); }

  Token MakeToken(TokenType type) {
    if (type == TOKEN_EOF) {
      start_++;
      return Token{type, -1, -1, -1};
    }
    return Token{type, start_, current_ - start_, line_};
  }

  Token MakeErrorToken(const string& msg) {
    (void)msg;
    return Token{TOKEN_ERROR, -1, -1, -1};
  }

  char Advance() {
    CHECK(!AtEnd());
    return source_[current_++];
  }

  char Peek() { return source_[current_]; }

  char PeekNext() { return source_[current_ + 1]; }

  string GetLexeme() {
    if (start_ == current_) return "";
    CHECK(start_ < source_.size() && start_ < current_);
    return source_.substr(start_, current_ - start_);
  }

  string GetLexeme(Token tk) {
    if (tk.type == TOKEN_EOF) return "EOF";
    return source_.substr(tk.start, tk.length);
  }

  bool Match(char c) {
    bool ok = !AtEnd() && source_[current_] == c;
    if (ok) current_++;
    return ok;
  }

  int StartPos() { return start_; }
  int CurrentPos() { return current_; }

  void SkipWhitespace();
  Token ProcessString();
  Token ProcessNumber();

  TokenType CheckKeyword(const string& key, TokenType type);
  TokenType IdentifierType();
  Token ProcessIdentifierKeyword();

 private:
  string source_;
  int start_;
  int current_;
  int line_;
};

}  // namespace xyxy

#endif  // XYXY_SCANNER_H_
