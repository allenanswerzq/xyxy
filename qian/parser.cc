#include "parser.h"

#include "inst.h"

namespace qian {

REGISTER_PREC_RULE(TOKEN_LEFT_PAREN)
  .Prefix_Rule(&Parser::parse_grouping)
  .Infix_Rule(nullptr)
  .Prec_Order(PREC_NONE);

REGISTER_PREC_RULE(TOKEN_RIGHT_PAREN);
REGISTER_PREC_RULE(TOKEN_LEFT_BRACE);
REGISTER_PREC_RULE(TOKEN_RIGHT_BRACE);
REGISTER_PREC_RULE(TOKEN_COMMA      );
REGISTER_PREC_RULE(TOKEN_DOT        );

REGISTER_PREC_RULE(TOKEN_MINUS      )
  .Prefix_Rule(&Parser::parse_unary)
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_TERM);

REGISTER_PREC_RULE(TOKEN_PLUS       )
  .Prefix_Rule(nullptr)
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_TERM);

REGISTER_PREC_RULE(TOKEN_SEMICOLON  );
REGISTER_PREC_RULE(TOKEN_SLASH      )
  .Prefix_Rule(nullptr)
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_FACTOR);

REGISTER_PREC_RULE(TOKEN_STAR       )
  .Prefix_Rule(nullptr)
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_FACTOR);

REGISTER_PREC_RULE(TOKEN_BANG       )
  .Prefix_Rule(&Parser::parse_unary);

REGISTER_PREC_RULE(TOKEN_BANG_EQUAL )
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_EQUALITY);

REGISTER_PREC_RULE(TOKEN_EQUAL      );

REGISTER_PREC_RULE(TOKEN_EQUAL_EQUAL)
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_EQUALITY);

REGISTER_PREC_RULE(TOKEN_GREATER    )
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_GREATER_EQUAL)
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_LESS       )
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_LESS_EQUAL )
  .Infix_Rule(&Parser::parse_binary)
  .Prec_Order(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_IDENTIFIER );
REGISTER_PREC_RULE(TOKEN_STRING     );

REGISTER_PREC_RULE(TOKEN_NUMBER     )
  .Prefix_Rule(&Parser::parse_number);

REGISTER_PREC_RULE(TOKEN_AND        );
REGISTER_PREC_RULE(TOKEN_CLASS      );
REGISTER_PREC_RULE(TOKEN_ELSE       );
REGISTER_PREC_RULE(TOKEN_FALSE      )
  .Prefix_Rule(&Parser::parse_literal);

REGISTER_PREC_RULE(TOKEN_FOR        );
REGISTER_PREC_RULE(TOKEN_FUN        );
REGISTER_PREC_RULE(TOKEN_IF         );

REGISTER_PREC_RULE(TOKEN_NIL        )
  .Prefix_Rule(&Parser::parse_literal);

REGISTER_PREC_RULE(TOKEN_OR         );
REGISTER_PREC_RULE(TOKEN_PRINT      );
REGISTER_PREC_RULE(TOKEN_RETURN     );
REGISTER_PREC_RULE(TOKEN_SUPER      );
REGISTER_PREC_RULE(TOKEN_THIS       );

REGISTER_PREC_RULE(TOKEN_TRUE       )
  .Prefix_Rule(&Parser::parse_literal);

REGISTER_PREC_RULE(TOKEN_VAR        );
REGISTER_PREC_RULE(TOKEN_WHILE      );
REGISTER_PREC_RULE(TOKEN_ERROR      );
REGISTER_PREC_RULE(TOKEN_EOF        );
REGISTER_PREC_RULE(TOKEN_NONE       );

void Parser::advance() {
  prev_ = curr_;
  while (true) {
    curr_ = scanner_->ScanToken();
    LOG(INFO) << "ScanToken: " << to_string(curr_);
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

void Parser::emit_byte(uint8 byte) {
  GetChunk()->WriteChunk(byte, prev_.line);
}

void Parser::emit_return() {
  emit_byte(OP_RETURN);
}

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

void Parser::parse_number() {
  double val = strtod(to_string(prev_).c_str(), nullptr);
  emit_constant(QIAN_NUMBER(val));
}

void Parser::parse_expression() {
  parse_with_prec_order(PREC_ASSIGNMENT);
}

void Parser::parse_grouping() {
  parse_expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after parse_expression.");
}

void Parser::parse_unary() {
  TokenType op_type = prev_.type;
  parse_with_prec_order(PREC_UNARY);
  if (op_type == TOKEN_MINUS) {
    emit_byte(OP_NEGATE);
  }
  else if (op_type == TOKEN_BANG) {
    emit_byte(OP_NOT);
  }
  else {
    CHECK(false) << "Unrecognized type: " << op_type;
  }
}

void Parser::parse_binary() {
  TokenType op_type = prev_.type;

  PrecRule* rule = get_rule(op_type);
  parse_with_prec_order(PrecOrder(rule->prec_order + 1));

  if (op_type == TOKEN_AND)           emit_byte(OP_ADD);
  if (op_type == TOKEN_MINUS)         emit_byte(OP_SUB);
  if (op_type == TOKEN_STAR)          emit_byte(OP_MUL);
  if (op_type == TOKEN_SLASH)         emit_byte(OP_DIV);
  if (op_type == TOKEN_BANG_EQUAL)    emit_byte(OP_EQUAL, OP_NOT);
  if (op_type == TOKEN_EQUAL_EQUAL)   emit_byte(OP_EQUAL);
  if (op_type == TOKEN_GREATER)       emit_byte(OP_GREATER);
  if (op_type == TOKEN_GREATER_EQUAL) emit_byte(OP_LESS, OP_NOT);
  if (op_type == TOKEN_LESS)          emit_byte(OP_LESS);
  if (op_type == TOKEN_LESS_EQUAL)    emit_byte(OP_GREATER, OP_NOT);
  CHECK(false) << "Unknown type: " << op_type;
}

void Parser::parse_literal() {
  if (prev_.type == TOKEN_FALSE)  emit_byte(OP_FALSE);
  if (prev_.type == TOKEN_TRUE)   emit_byte(OP_TRUE);
  if (prev_.type == TOKEN_NIL)    emit_byte(OP_NIL);
  CHECK(false) << "Unreachable.";
}

void Parser::parse_with_prec_order(PrecOrder prec_order) {
  advance();

  auto prev_rule = get_rule(prev_.type);
  ParseFunc prefix_rule = prev_rule->prefix_rule;
  CHECK(prefix_rule) << "Expect parse_expression.";
  prefix_rule(this);

  auto curr_rule = get_rule(curr_.type);
  while (curr_rule->prec_order >= prec_order) {
    advance();
    ParseFunc infix_rule = curr_rule->infix_rule;
    infix_rule(this);
  }
}

}  // namespace qian
