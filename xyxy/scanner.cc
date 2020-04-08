#include "xyxy/scanner.h"

namespace xyxy {

bool operator==(const Token& a, const Token& b) {
  bool r = a.type == b.type;
  r &= a.start == b.start;
  r &= a.length == b.length;
  r &= a.line == b.line;
  return r;
}

bool operator!=(const Token& a, const Token& b) { return !(a == b); }

string Token::ToString() {
  switch (type) {
    case TOKEN_LEFT_PAREN:
      return "(";
    case TOKEN_RIGHT_PAREN:
      return ")";
    case TOKEN_LEFT_BRACE:
      return "{";
    case TOKEN_RIGHT_BRACE:
      return "}";
    case TOKEN_COMMA:
      return ",";
    case TOKEN_DOT:
      return ".";
    case TOKEN_MINUS:
      return "-";
    case TOKEN_PLUS:
      return "+";
    case TOKEN_SEMICOLON:
      return ";";
    case TOKEN_SLASH:
      return "/";
    case TOKEN_STAR:
      return "*";

    case TOKEN_BANG:
      return "!";
    case TOKEN_BANG_EQUAL:
      return "!=";
    case TOKEN_EQUAL:
      return "=";
    case TOKEN_EQUAL_EQUAL:
      return "==";
    case TOKEN_GREATER:
      return ">";
    case TOKEN_GREATER_EQUAL:
      return ">=";
    case TOKEN_LESS:
      return "<";
    case TOKEN_LESS_EQUAL:
      return "<=";

    case TOKEN_IDENTIFIER:
      return "identifier";
    case TOKEN_STRING:
      return "string";
    case TOKEN_NUMBER:
      return "number";

    case TOKEN_IF:
      return "if";
    case TOKEN_AND:
      return "and";
    case TOKEN_ELSE:
      return "else";
    case TOKEN_FALSE:
      return "false";
    case TOKEN_FUN:
      return "fun";
    case TOKEN_FOR:
      return "for";
    case TOKEN_NIL:
      return "nil";
    case TOKEN_OR:
      return "or";
    case TOKEN_CLASS:
      return "class";
    case TOKEN_PRINT:
      return "print";
    case TOKEN_RETURN:
      return "return";
    case TOKEN_SUPER:
      return "super";
    case TOKEN_THIS:
      return "this";
    case TOKEN_TRUE:
      return "true";
    case TOKEN_VAR:
      return "var";
    case TOKEN_WHILE:
      return "while";

    case TOKEN_NEWLINE:
      return "\n";
    case TOKEN_WHITESPACE:
      return "white";

    case TOKEN_ERROR:
      return "error";
    case TOKEN_EOF:
      return "eof";

    case TOKEN_NONE:
      return "None";
    default:
      break;
  }
  CHECK(false) << "Not recognized token: " << type;
}

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

static bool is_digit(char c) { return '0' <= c && c <= '9'; }

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
  switch (c) {
    case 'a':
      return check_keyword("and", TOKEN_AND);
    case 'c':
      return check_keyword("class", TOKEN_CLASS);
    case 'e':
      return check_keyword("else", TOKEN_ELSE);
    case 'i':
      return check_keyword("if", TOKEN_IF);
    case 'n':
      return check_keyword("nil", TOKEN_NIL);
    case 'o':
      return check_keyword("or", TOKEN_OR);
    case 'p':
      return check_keyword("print", TOKEN_PRINT);
    case 'r':
      return check_keyword("return", TOKEN_RETURN);
    case 's':
      return check_keyword("super", TOKEN_SUPER);
    case 'v':
      return check_keyword("var", TOKEN_VAR);
    case 'w':
      return check_keyword("while", TOKEN_WHILE);
    case 'f': {
      char n = source_[start_ + 1];
      switch (n) {
        case 'a':
          return check_keyword("false", TOKEN_FALSE);
        case 'o':
          return check_keyword("for", TOKEN_FOR);
        case 'u':
          return check_keyword("fun", TOKEN_FUN);
        default:
          break;
      }
      break;
    }
    case 't': {
      char n = source_[start_ + 1];
      switch (n) {
        case 'h':
          return check_keyword("this", TOKEN_THIS);
        case 'r':
          return check_keyword("true", TOKEN_TRUE);
        default:
          break;
      }
      break;
    }
    default:
      // TODO(zq7): error handling maybe.
      break;
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
  switch (c) {
    case ',':
      return make_token(TOKEN_COMMA);
    case '.':
      return make_token(TOKEN_DOT);
    case '-':
      return make_token(TOKEN_MINUS);
    case '+':
      return make_token(TOKEN_PLUS);
    case '/':
      return make_token(TOKEN_SLASH);
    case '*':
      return make_token(TOKEN_STAR);
    case '(':
      return make_token(TOKEN_LEFT_PAREN);
    case ')':
      return make_token(TOKEN_RIGHT_PAREN);
    case '{':
      return make_token(TOKEN_LEFT_BRACE);
    case '}':
      return make_token(TOKEN_RIGHT_BRACE);
    case ';':
      return make_token(TOKEN_SEMICOLON);
    case '!':
      return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '\n': {
      line_++;
      advance();
      break;
    }
    case '"':
      return process_string();
    default:
      if (is_digit(c)) return process_number();
      if (is_alpha(c)) return process_identifier_keyword();
      break;
  }
  return make_error_token("Unexpected characters met.");
}

string Scanner::GetSource(int start, int end) {
  string ret;
  ret.append(std::to_string(start));
  ret.append("-");
  ret.append(std::to_string(end));
  ret.append(" `");
  ret.append(source_.substr(start, end - start));
  ret.append("`");
  return ret;
}

string Scanner::strip_source(const string& str) {
  int n = str.size();
  int i = 0;
  while (i < n && (str[i] == '\n' || str[i] == ' ')) {
    i++;
  }
  int j = n - 1;
  while (j > i && (str[j] == '\n' || str[j] == ' ')) {
    j--;
  }
  return str.substr(i, j - i + 1);
}
}  // namespace xyxy
