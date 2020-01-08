#include "parser.h"

#include "inst.h"

namespace qian {

// NOTE: the order must be the same as token definition
REGISTER_PREC_RULE(TOKEN_LEFT_PAREN)
    .Prefix_Rule(&Parser::parse_grouping)
    .Infix_Rule(nullptr)
    .Prec_Order(PREC_NONE);

REGISTER_PREC_RULE(TOKEN_RIGHT_PAREN);
REGISTER_PREC_RULE(TOKEN_LEFT_BRACE);
REGISTER_PREC_RULE(TOKEN_RIGHT_BRACE);
REGISTER_PREC_RULE(TOKEN_COMMA);
REGISTER_PREC_RULE(TOKEN_DOT);

REGISTER_PREC_RULE(TOKEN_MINUS)
    .Prefix_Rule(&Parser::parse_unary)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_TERM);

REGISTER_PREC_RULE(TOKEN_PLUS)
    .Prefix_Rule(nullptr)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_TERM);

REGISTER_PREC_RULE(TOKEN_SEMICOLON);
REGISTER_PREC_RULE(TOKEN_SLASH)
    .Prefix_Rule(nullptr)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_FACTOR);

REGISTER_PREC_RULE(TOKEN_STAR)
    .Prefix_Rule(nullptr)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_FACTOR);

REGISTER_PREC_RULE(TOKEN_BANG).Prefix_Rule(&Parser::parse_unary);

REGISTER_PREC_RULE(TOKEN_BANG_EQUAL)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_EQUALITY);

REGISTER_PREC_RULE(TOKEN_EQUAL);

REGISTER_PREC_RULE(TOKEN_EQUAL_EQUAL)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_EQUALITY);

REGISTER_PREC_RULE(TOKEN_GREATER)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_GREATER_EQUAL)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_LESS)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_LESS_EQUAL)
    .Infix_Rule(&Parser::parse_binary)
    .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_IDENTIFIER);
REGISTER_PREC_RULE(TOKEN_STRING);

REGISTER_PREC_RULE(TOKEN_NUMBER).Prefix_Rule(&Parser::parse_number);

REGISTER_PREC_RULE(TOKEN_AND);
REGISTER_PREC_RULE(TOKEN_IF);
REGISTER_PREC_RULE(TOKEN_ELSE);
REGISTER_PREC_RULE(TOKEN_FALSE).Prefix_Rule(&Parser::parse_literal);

REGISTER_PREC_RULE(TOKEN_FUN);
REGISTER_PREC_RULE(TOKEN_FOR);

REGISTER_PREC_RULE(TOKEN_NIL).Prefix_Rule(&Parser::parse_literal);

REGISTER_PREC_RULE(TOKEN_OR);
REGISTER_PREC_RULE(TOKEN_CLASS);
REGISTER_PREC_RULE(TOKEN_PRINT);
REGISTER_PREC_RULE(TOKEN_RETURN);
REGISTER_PREC_RULE(TOKEN_SUPER);
REGISTER_PREC_RULE(TOKEN_THIS);

REGISTER_PREC_RULE(TOKEN_TRUE).Prefix_Rule(&Parser::parse_literal);

REGISTER_PREC_RULE(TOKEN_VAR);
REGISTER_PREC_RULE(TOKEN_WHILE);
REGISTER_PREC_RULE(TOKEN_NEWLINE);
REGISTER_PREC_RULE(TOKEN_WHITESPACE);

REGISTER_PREC_RULE(TOKEN_ERROR);
REGISTER_PREC_RULE(TOKEN_EOF);

REGISTER_PREC_RULE(TOKEN_NONE).Prefix_Rule(&Parser::parse_grouping);

void Parser::advance() {
  prev_ = curr_;
  while (true) {
    curr_ = scanner_->ScanToken();
    // LOG(INFO) << "ScanToken: " << to_string(curr_);
    if (curr_.type != TOKEN_ERROR) {
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

void Parser::emit_byte(uint8 byte) { GetChunk()->WriteChunk(byte, prev_.line); }

void Parser::emit_return() { emit_byte(OP_RETURN); }

void Parser::emit_byte(uint8 byte1, uint8 byte2) {
  emit_byte(byte1);
  emit_byte(byte2);
}

int Parser::make_constant(Value val) {
  // Get the index after adding this val into chunk.
  int idx = GetChunk()->AddValue(val);
  CHECK(idx <= UINT8_MAX) << "Not expect to many consts in one chunk.";
  return idx;
}

void Parser::emit_constant(Value val) {
  emit_byte(OP_CONSTANT, make_constant(val));
}

#define DEBUG_PARSER_ENTER(type)      \
  DebugParser debug;                  \
  debug.enter_pos = prev_.start;      \
  debug.parse_depth = parse_depth_++; \
  debug.msg = type

#define DEBUG_PARSER_EXIT()                    \
  debug.exit_pos = prev_.start + prev_.length; \
  parse_depth_--;                              \
  debug_parser(debug)

void Parser::debug_parser(DebugParser debug) {
  // TODO(zq7): Make debug drawing looks more beautiful.
  string prefix(debug.parse_depth * 3, '-');
  CHECK(debug.enter_pos < debug.exit_pos);
  string source = scanner_->interval_source(debug.enter_pos, debug.exit_pos);
  LOG(INFO) << prefix << std::to_string(debug.parse_depth) << "|   " << source;
}

void Parser::parse_number() {
  DEBUG_PARSER_ENTER("number");

  double val = strtod(to_string(prev_).c_str(), nullptr);
  emit_constant(QIAN_NUMBER(val));

  DEBUG_PARSER_EXIT();
}

void Parser::parse_expression() {
  DEBUG_PARSER_ENTER("express");

  parse_with_prec_order(PREC_ASSIGNMENT);

  DEBUG_PARSER_EXIT();
}

void Parser::parse_grouping() {
  DEBUG_PARSER_ENTER("group");

  parse_expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parse_expression.");

  DEBUG_PARSER_EXIT();
}

void Parser::parse_unary() {
  DEBUG_PARSER_ENTER("unary");

  TokenType op_type = prev_.type;
  parse_with_prec_order(PREC_UNARY);
  switch (op_type) {
    case TOKEN_MINUS:
      emit_byte(OP_NEGATE);
      break;
    case TOKEN_BANG:
      emit_byte(OP_NOT);
      break;
    default:
      CHECK(false) << "Unreachable.";
      break;
  }

  DEBUG_PARSER_EXIT();
}

void Parser::parse_binary() {
  DEBUG_PARSER_ENTER("binary");

  TokenType op_type = prev_.type;
  PrecRule* rule = get_rule(op_type);
  parse_with_prec_order(PrecOrder(rule->prec_order + 1));
  switch (op_type) {
    case TOKEN_PLUS:
      emit_byte(OP_ADD);
      break;
    case TOKEN_MINUS:
      emit_byte(OP_SUB);
      break;
    case TOKEN_STAR:
      emit_byte(OP_MUL);
      break;
    case TOKEN_SLASH:
      emit_byte(OP_DIV);
      break;
    case TOKEN_BANG_EQUAL:
      emit_byte(OP_EQUAL, OP_NOT);
      break;
    case TOKEN_EQUAL_EQUAL:
      emit_byte(OP_EQUAL);
      break;
    case TOKEN_GREATER:
      emit_byte(OP_GREATER);
      break;
    case TOKEN_GREATER_EQUAL:
      emit_byte(OP_LESS, OP_NOT);
      break;
    case TOKEN_LESS:
      emit_byte(OP_LESS);
      break;
    case TOKEN_LESS_EQUAL:
      emit_byte(OP_GREATER, OP_NOT);
      break;
    default:
      CHECK(false) << "Unreachable." << ToString(op_type);
      break;
  }

  DEBUG_PARSER_EXIT();
}

void Parser::parse_literal() {
  DEBUG_PARSER_ENTER("literal");

  switch (prev_.type) {
    case TOKEN_FALSE:
      emit_byte(OP_FALSE);
      break;
    case TOKEN_TRUE:
      emit_byte(OP_TRUE);
      break;
    case TOKEN_NIL:
      emit_byte(OP_NIL);
      break;
    default:
      CHECK(false) << "Unreachable.";
      break;
  }

  DEBUG_PARSER_EXIT();
}

void Parser::parse_with_prec_order(PrecOrder prec_order) {
  DEBUG_PARSER_ENTER("prec_order");

  advance();

  auto prev_rule = get_rule(prev_.type);
  CHECK(prev_rule);
  ParseFunc prefix_rule = prev_rule->prefix_rule;
  CHECK(prefix_rule);

  prefix_rule(this);

  while (get_rule(curr_.type)->prec_order >= prec_order) {
    advance();
    ParseFunc infix_rule = get_rule(prev_.type)->infix_rule;
    infix_rule(this);
  }

  DEBUG_PARSER_EXIT();
}

}  // namespace qian
