#ifndef QIAN_SCANNER_H_
#define QIAN_SCANNER_H_

#include "base.h"
#include "debug.h"

namespace qian {

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

typedef struct {
  TokenType type;
  int start;
  int length;
  int line;
} Token;

inline bool operator==(const Token& a, const Token& b) {
  bool r = a.type == b.type;
  r &= a.start == b.start;
  r &= a.length == b.length;
  r &= a.line == b.line;
  return r;
}

inline bool operator!=(const Token& a, const Token& b) { return !(a == b); }

string ToString(TokenType tt);

class Scanner {
 public:
  Scanner(const string& source) {
    source_ = Strip(source);
    start_ = 0;
    current_ = 0;
    line_ = 1;
  }

  Token ScanToken();

  bool at_end() { return current_ == source_.size(); }

  Token make_token(TokenType type) {
    if (type == TOKEN_EOF) {
      start_++;
      return Token{type, -1, -1, -1};
    }
    return Token{type, start_, current_ - start_, line_};
  }

  Token make_error_token(const string& msg) {
    (void)msg;
    return Token{TOKEN_ERROR, -1, -1, -1};
  }

  char advance() {
    CHECK(!at_end());
    return source_[current_++];
  }

  char peek() { return source_[current_]; }

  char peek_next() { return source_[current_ + 1]; }

  string get_lexeme() {
    if (start_ == current_) return "";
    CHECK(start_ < source_.size() && start_ < current_);
    return source_.substr(start_, current_ - start_);
  }

  string get_lexeme(Token tk) {
    if (tk.type == TOKEN_EOF) return "EOF";
    return source_.substr(tk.start, tk.length);
  }

  bool match(char c) {
    bool ok = !at_end() && source_[current_] == c;
    if (ok) current_++;
    return ok;
  }

  int start_pos() { return start_; }
  int current_pos() { return current_; }

  string interval_source(int start, int end);

  void skip_whitespace();
  Token process_string();
  Token process_number();

  TokenType check_keyword(const string& key, TokenType type);
  TokenType identifier_type();
  Token process_identifier_keyword();

 private:
  string source_;
  int start_;
  int current_;
  int line_;
};

}  // namespace qian

#endif  // QIAN_SCANNER_H_
