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

void Scanner::SkipWhitespace() {
  for (;;) {
    char c = Peek();
    if (c == ' ' || c == '\r' || c == '\t') {
      Advance();
    }
    else if (c == '/') {
      // Handle comments.
      if (PeekNext() == '/') {
        while (!AtEnd() && Peek() != '\n') {
          Advance();
        }
      }
      else {
        // Not a comment, simply return.
        return;
      }
    }
    else if (c == '\n') {
      line_++;
      Advance();
    }
    else {
      return;
    }
  }
}

Token Scanner::ProcessString() {
  // Skip the first quote sign.
  Advance();
  while (!AtEnd() && Peek() != '"') {
    Advance();
  }
  if (AtEnd() || Peek() != '"') {
    return MakeErrorToken("Unterminated string.");
  }
  // Skip the second quote sign.
  Advance();
  return MakeToken(TokenType::TOKEN_STRING);
}

static bool IsDigit(char c) { return '0' <= c && c <= '9'; }

static bool IsAlpha(char c) {
  bool x = ('a' <= c && c <= 'z');
  x |= ('A' <= c && c <= 'Z');
  x |= (c == '_');
  return x;
}

Token Scanner::ProcessNumber() {
  while (!AtEnd() && IsDigit(Peek())) {
    Advance();
  }
  if (!AtEnd() && Peek() == '.' && IsDigit(PeekNext())) {
    // Consume the '.' char.
    Advance();
    while (!AtEnd() && IsDigit(Peek())) {
      Advance();
    }
  }
  return MakeToken(TOKEN_NUMBER);
}

TokenType Scanner::CheckKeyword(const string& key, TokenType type) {
  return GetLexeme() == key ? type : TOKEN_IDENTIFIER;
}

TokenType Scanner::IdentifierType() {
  char c = source_[start_];
  switch (c) {
    case 'a':
      return CheckKeyword("and", TOKEN_AND);
    case 'c':
      return CheckKeyword("class", TOKEN_CLASS);
    case 'i':
      return CheckKeyword("if", TOKEN_IF);
    case 'n':
      return CheckKeyword("nil", TOKEN_NIL);
    case 'o':
      return CheckKeyword("or", TOKEN_OR);
    case 'p':
      return CheckKeyword("print", TOKEN_PRINT);
    case 'r':
      return CheckKeyword("return", TOKEN_RETURN);
    case 's':
      return CheckKeyword("super", TOKEN_SUPER);
    case 'v':
      return CheckKeyword("var", TOKEN_VAR);
    case 'w':
      return CheckKeyword("while", TOKEN_WHILE);
    case 'f': {
      char n = source_[start_ + 1];
      switch (n) {
        case 'a':
          return CheckKeyword("false", TOKEN_FALSE);
        case 'o':
          return CheckKeyword("for", TOKEN_FOR);
        case 'u':
          return CheckKeyword("fun", TOKEN_FUN);
        default:
          break;
      }
    }
    case 'e': {
      char n = source_[start_ + 2];
      switch (n) {
        case 'i':
          return CheckKeyword("elif", TOKEN_ELIF);
        case 's':
          return CheckKeyword("else", TOKEN_ELSE);
        default:
          break;
      }
    }
    case 't': {
      char n = source_[start_ + 1];
      switch (n) {
        case 'h':
          return CheckKeyword("this", TOKEN_THIS);
        case 'r':
          return CheckKeyword("true", TOKEN_TRUE);
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

Token Scanner::ProcessIdentifierKeyword() {
  while (IsAlpha(Peek()) || IsDigit(Peek())) {
    Advance();
  }
  return MakeToken(IdentifierType());
}

Token Scanner::ScanToken() {
  if (AtEnd()) {
    return MakeToken(TOKEN_EOF);
  }
  SkipWhitespace();
  start_ = current_;
  char c = Advance();
  switch (c) {
    case ',':
      return MakeToken(TOKEN_COMMA);
    case '.':
      return MakeToken(TOKEN_DOT);
    case '-':
      return MakeToken(TOKEN_MINUS);
    case '+':
      return MakeToken(TOKEN_PLUS);
    case '/':
      return MakeToken(TOKEN_SLASH);
    case '*':
      return MakeToken(TOKEN_STAR);
    case '(':
      return MakeToken(TOKEN_LEFT_PAREN);
    case ')':
      return MakeToken(TOKEN_RIGHT_PAREN);
    case '{':
      return MakeToken(TOKEN_LEFT_BRACE);
    case '}':
      return MakeToken(TOKEN_RIGHT_BRACE);
    case ';':
      return MakeToken(TOKEN_SEMICOLON);
    case '!':
      return MakeToken(Match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return MakeToken(Match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return MakeToken(Match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return MakeToken(Match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
      return ProcessString();
    default:
      if (IsDigit(c)) return ProcessNumber();
      if (IsAlpha(c)) return ProcessIdentifierKeyword();
      break;
  }
  return MakeErrorToken("Unexpected characters met.");
}

string Scanner::GetSource(int start, int end) {
  return source_.substr(start, end - start);
}

string Scanner::StripSource(const string& str) {
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
