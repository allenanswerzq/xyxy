#ifndef QIAN_SCANNER_H_
#define QIAN_SCANNER_H_

#include "base.h"

namespace qian {

typedef enum {
  // Single character tokens.
  TOKEN_LEFT_PAREN,     // "("
  TOKEN_RIGHT_PAREN,    // ")"
  TOKEN_LEFT_BRACE,     // "{"
  TOKEN_RIGHT_BRACE,    // "}"
  TOKEN_COMMA,          // ","
  TOKEN_DOT,            // "."
  TOKEN_MINUS,          // "-"
  TOKEN_PLUS,           // "+"
  TOKEN_SEMICOLON,      // ";"
  TOKEN_SLASH,          // "/"
  TOKEN_STAR,           // "*"

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
  TOKEN_IDENTIFIER,     // "identifier"
  TOKEN_STRING,         // "string"
  TOKEN_NUMBER,         // "number"

  // Keywords
  TOKEN_AND,            // "and"
  TOKEN_IF,             // "if"
  TOKEN_ELSE,           // "else"
  TOKEN_FALSE,          // "false"
  TOKEN_FUN,            // "fun"
  TOKEN_FOR,            // "for"
  TOKEN_NIL,            // "nil"
  TOKEN_OR,             // "or"
  TOKEN_CLASS,          // "class"
  TOKEN_PRINT,          // "print"
  TOKEN_RETURN,         // "return"
  TOKEN_SUPER,          // "super"
  TOKEN_THIS,           // "this"
  TOKEN_TRUE,           // "true"
  TOKEN_VAR,            // "var"
  TOKEN_WHILE,          // "while"

  TOKEN_NEWLINE,        // "\n"
  TOKEN_WHITESPACE,     // "white"

  TOKEN_ERROR,          // "error"
  TOKEN_EOF,            // "eof"
} TokenType;

struct Token {
  TokenType type_;
  int start_;
  int length_;
  int line_;
};

// This may looks ugly.
struct ErrorToken : public Token {
  TokenType type_;
  int line_;
  string msg_;
  ErrorToken(TokenType type, int line, string msg)
    : type_(type), line_(line), msg_(msg) {}
};

class Scanner {
 public:
  Scanner(const string& source) {
    source_ = source;
    start_ = 0;
    current_ = 0;
    line_ = 1;
  }

  Token ScanToken();

  bool at_end() {
    return current_ == source_.size();
  }

  Token make_token(TokenType type) {
    return Token{type, start_, current_ - start_, line_};
  }

  Token make_error_token(const string& msg) {
    return ErrorToken(TOKEN_ERROR, line_, msg);
  }

  char advance() {
    return source_[current_++];
  }

  char peek() {
    return source_[current_];
  }

  char peek_next() {
    return source_[current_ + 1];
  }

  string get_lexeme() {
    return source_.substr(start_, current_ - start_);
  }

  bool match(char c) {
    bool ok = !at_end() && source_[current_] == c;
    if (ok) current_++;
    return ok;
  }

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

#endif // QIAN_SCANNER_H_
