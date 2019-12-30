#include "parser.h"

#include "inst.h"

namespace qian {

void Parser::advance() {
  prev_ = curr_;
  while (true) {
    curr_ = scanner_.ScanToken();
    if (curr_.type != TokenType.ERROR) {
      break;
    }
    // ShowError(curr_);
  }
}

void Parser::consume(TokenType type, const string& msg) {
  if (curr_.type == type) {
    advance();
    return;
  }
  // ShowError(msg);
}

void Parser::emit_byte(uint8 byte) [
  GetChunk()->WriteChunk(byte, prev_.line);
}

void Parser::emit_return(uint8 byte) [
  emit_byte(OP_RETURN);
}

void Parser::emit_byte(uint8 byte1, uint8 byte2) [
  emit_byte(byte1);
  emit_byte(byte2);
}

int Parser::make_constant(Value val) {
  // Get the index after adding this val into chunk.
  int idx = GetChunk()->AddValue(val);
  // TODO(zq7): rewrite with VLOG later maybe.
  // if (idx > UINT8_MAX) {
  //   ShowError("Not expect to many consts in one chunk.");
  // }
  return idx;
}

void Parser::emit_constant(Value val) {
  emit_bytes(OP_CONSTANT, make_constant(val));
}

void Parser::number() {
  Value val = strtod(get_lexeme(prev_));
  emit_constant(val);
}

void Parser::grouping() {
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Parser::unary() {
  TokenType op_type = prev_.type;
  expression();
  if (op_type == TOKEN_MINUS) {
    emit_byte(OP_NEGATE);
  }
}

void Parser::binary() {
  TokenType op_type = prev_.type;
  ParseRule* rule = get_rule(op_type);
  ParsePrecedence(rule->precedence + 1);
}

void Parser::parse_with_prec_order(PrecOrder prec_order) {
  advance();
  ParseFunc prefix_rule = get_rule_func(prev_.type);
  // TODO(zq7): Dies if not true, make a future check on this one.
  CHECK(prefix_rule) << "Expect expression.";
  prefix_rule();
  while (prec_order <= get_rule_order(prev_.type)) {
    advance();
    ParseFunc infix_rule = get_rule_func(prev_.type);
    CHECK(infix_rule) << "Expect expression.";
    infix_rule();
  }
}

}  // namespace qian
