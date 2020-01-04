#include "scanner.h"

namespace qian {

void Scanner::skip_whitespace() {
  for (;;) {
    char c = peek();
    if (c == ' ' || c == '\r' || c == '\t') {
      advance();
    } else if (c == '/') {
      // Handle comments.
      if (peek_next() == '/') {
        while (!at_end() && peek() != '\n') {
          advance();
        }
      } else {
        // Not a comment, simply return.
        return;
      }
    } else {
      return;
    }
  }
}

Token Scanner::process_string() {
  // Skip the first quote sign.
  advance();
  while (!at_end() && peek() != '"') {
    advance();
  }
  if (at_end() || peek() != '"') {
    return make_error_token("Unterminated string.");
  }
  // Skip the second quote sign.
  advance();
  return make_token(TokenType::TOKEN_STRING);
}

static bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static bool is_alpha(char c) {
  bool x = ('a' <= c && c <= 'z');
  x |= ('A' <= c && c <= 'Z');
  x |= (c == '_');
  return x;
}

Token Scanner::process_number() {
  while (!at_end() && is_digit(peek())) {
    advance();
  }
  if (!at_end() && peek() == '.' && is_digit(peek_next())) {
    // Consume the '.' char.
    advance();
    while (!at_end() && is_digit(peek())) {
      advance();
    }
  }
  return make_token(TOKEN_NUMBER);
}

TokenType Scanner::check_keyword(const string& key, TokenType type) {
  return get_lexeme() == key ? type : TOKEN_IDENTIFIER;
}

TokenType Scanner::identifier_type() {
  char c = source_[start_];
  if (c == 'a') return check_keyword("and",     TOKEN_AND);
  if (c == 'c') return check_keyword("class",   TOKEN_CLASS);
  if (c == 'e') return check_keyword("else",    TOKEN_ELSE);
  if (c == 'i') return check_keyword("if",      TOKEN_IF);
  if (c == 'n') return check_keyword("nil",     TOKEN_NIL);
  if (c == 'o') return check_keyword("or",      TOKEN_OR);
  if (c == 'p') return check_keyword("print",   TOKEN_PRINT);
  if (c == 'r') return check_keyword("return",  TOKEN_RETURN);
  if (c == 's') return check_keyword("super",   TOKEN_SUPER);
  if (c == 'v') return check_keyword("var",     TOKEN_VAR);
  if (c == 'w') return check_keyword("while",   TOKEN_WHILE);
  if (c == 'f') {
    char n = source_[start_ + 1];
    if (n == 'a') return check_keyword("false", TOKEN_FALSE);
    if (n == 'o') return check_keyword("for",   TOKEN_FOR);
    if (n == 'u') return check_keyword("fun",   TOKEN_FUN);
  }
  if (c == 't') {
    char n = source_[start_ + 1];
    if (n == 'h') return check_keyword("this",  TOKEN_THIS);
    if (n == 'r') return check_keyword("true",  TOKEN_TRUE);
  }
  return TOKEN_IDENTIFIER;
}

Token Scanner::process_identifier_keyword() {
  while (is_alpha(peek()) || is_digit(peek())) {
    advance();
  }
  return make_token(identifier_type());
}

Token Scanner::ScanToken() {
  if (at_end()) {
    return make_token(TOKEN_EOF);
  }
  skip_whitespace();
  start_ = current_;
  char c = advance();
  if (c == ',') return make_token(TOKEN_COMMA);
  if (c == '.') return make_token(TOKEN_DOT);
  if (c == '-') return make_token(TOKEN_MINUS);
  if (c == '+') return make_token(TOKEN_PLUS);
  if (c == '/') return make_token(TOKEN_SLASH);
  if (c == '*') return make_token(TOKEN_STAR);
  if (c == '(') return make_token(TOKEN_LEFT_PAREN);
  if (c == ')') return make_token(TOKEN_RIGHT_PAREN);
  if (c == '{') return make_token(TOKEN_LEFT_BRACE);
  if (c == '}') return make_token(TOKEN_RIGHT_BRACE);
  if (c == ';') return make_token(TOKEN_SEMICOLON);
  if (c == '!') return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  if (c == '=') return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  if (c == '<') return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  if (c == '>') return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  if (c == '\n') { line_++; advance(); }
  if (c == '"') return process_string();
  if (is_digit(c)) return process_number();
  if (is_alpha(c)) return process_identifier_keyword();
  return make_error_token("Unexpected characters met.");
}

} // namespace qian
