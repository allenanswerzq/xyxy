#ifndef QIAN_SCANNER_H_
#define QIAN_SCANNER_H_

#include "base.h"

namespace qian {

enum class TokenType {
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

  TOKEN_ERROR,           // error
  TOKEN_EOF,             // "eof"

};

struct Token {
  TokenType type;
  int start;
  int length;
  int line;
};

class Scanner {
 public:
  Scanner(const string& source) {
    source_ = source;
    start_ = 0;
    current_ = 0;
    line_ = 1;
  }

 private:
  string source_;
  int start_;
  int current_;
  int line_;
};

} // namespace qian


#endif // QIAN_SCANNER_H_
