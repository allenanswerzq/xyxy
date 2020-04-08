#include "xyxy/compiler.h"

#include "xyxy/inst.h"
#include "xyxy/object.h"

namespace xyxy {

// NOTE: the order must be the same as token definition.
// TODO(zq7): this whole piece code looks very ugly.
// clang-format off
// (
REGISTER_PREC_RULE(TOKEN_LEFT_PAREN)
    .PrefixRule(&Compiler::ParseGrouping)
    .InfixRule(nullptr)
    .PrecOrder(PREC_NONE);

REGISTER_PREC_RULE(TOKEN_RIGHT_PAREN);
REGISTER_PREC_RULE(TOKEN_LEFT_BRACE);
REGISTER_PREC_RULE(TOKEN_RIGHT_BRACE);
REGISTER_PREC_RULE(TOKEN_COMMA);
REGISTER_PREC_RULE(TOKEN_DOT);

// -
REGISTER_PREC_RULE(TOKEN_MINUS)
    .PrefixRule(&Compiler::ParseUnary)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_TERM);

// +
REGISTER_PREC_RULE(TOKEN_PLUS)
    .PrefixRule(nullptr)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_TERM);

REGISTER_PREC_RULE(TOKEN_SEMICOLON);

// /
REGISTER_PREC_RULE(TOKEN_SLASH)
    .PrefixRule(nullptr)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_FACTOR);

// *
REGISTER_PREC_RULE(TOKEN_STAR)
    .PrefixRule(nullptr)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_FACTOR);

REGISTER_PREC_RULE(TOKEN_BANG)
    .PrefixRule(&Compiler::ParseUnary);

REGISTER_PREC_RULE(TOKEN_BANG_EQUAL)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_EQUALITY);

REGISTER_PREC_RULE(TOKEN_EQUAL);

REGISTER_PREC_RULE(TOKEN_EQUAL_EQUAL)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_EQUALITY);

REGISTER_PREC_RULE(TOKEN_GREATER)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_GREATER_EQUAL)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_LESS)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_LESS_EQUAL)
    .InfixRule(&Compiler::ParseBinary)
    .PrecOrder(PREC_COMPARISON);

REGISTER_PREC_RULE(TOKEN_IDENTIFIER);

REGISTER_PREC_RULE(TOKEN_STRING)
    .PrefixRule(&Compiler::ParseString);

REGISTER_PREC_RULE(TOKEN_NUMBER)
    .PrefixRule(&Compiler::ParseNumber);

REGISTER_PREC_RULE(TOKEN_AND);
REGISTER_PREC_RULE(TOKEN_IF);
REGISTER_PREC_RULE(TOKEN_ELSE);
REGISTER_PREC_RULE(TOKEN_FALSE)
    .PrefixRule(&Compiler::ParseLiteral);

REGISTER_PREC_RULE(TOKEN_FUN);
REGISTER_PREC_RULE(TOKEN_FOR);

REGISTER_PREC_RULE(TOKEN_NIL)
    .PrefixRule(&Compiler::ParseLiteral);

REGISTER_PREC_RULE(TOKEN_OR);
REGISTER_PREC_RULE(TOKEN_CLASS);
REGISTER_PREC_RULE(TOKEN_PRINT);
REGISTER_PREC_RULE(TOKEN_RETURN);
REGISTER_PREC_RULE(TOKEN_SUPER);
REGISTER_PREC_RULE(TOKEN_THIS);

REGISTER_PREC_RULE(TOKEN_TRUE)
    .PrefixRule(&Compiler::ParseLiteral);

REGISTER_PREC_RULE(TOKEN_VAR);
REGISTER_PREC_RULE(TOKEN_WHILE);
REGISTER_PREC_RULE(TOKEN_NEWLINE);
REGISTER_PREC_RULE(TOKEN_WHITESPACE);

REGISTER_PREC_RULE(TOKEN_ERROR);
REGISTER_PREC_RULE(TOKEN_EOF);

REGISTER_PREC_RULE(TOKEN_NONE)
    .PrefixRule(&Compiler::ParseGrouping);

// clang-format on

void Compiler::Advance() {
  prev_ = curr_;
  while (true) {
    curr_ = scanner_->ScanToken();
    VLOG(1) << "ScanToken: " << GetLexeme(curr_);
    if (curr_.type != TOKEN_ERROR) {
      break;
    } else {
      CHECK(false) << "Unrecogined toke: " << curr_.type;
    }
  }
}

void Compiler::Consume(TokenType type, const string& msg) {
  CHECK(curr_.type == type) << "Not expected token: " << GetLexeme(curr_);
  Advance();
  return;
}

void Compiler::EmitByte(uint8 byte) { GetChunk()->Write(byte, prev_.line); }

void Compiler::EmitReturn() { EmitByte(OP_RETURN); }

void Compiler::EmitByte(uint8 byte1, uint8 byte2) {
  EmitByte(byte1);
  EmitByte(byte2);
}

int Compiler::MakeConstant(Value val) {
  // Get the index after adding this val into chunk.
  int idx = GetChunk()->AddConstant(val);
  CHECK(idx <= UINT8_MAX) << "Not expect to many consts in one chunk.";
  return idx;
}

void Compiler::EmitConstant(Value val) {
  EmitByte(OP_CONSTANT, MakeConstant(val));
}

#define DEBUG_PARSER_ENTER(type)      \
  DebugParser debug;                  \
  debug.enter_pos = prev_.start;      \
  debug.parse_depth = parse_depth_++; \
  debug.msg = type

#define DEBUG_PARSER_EXIT()                    \
  debug.exit_pos = prev_.start + prev_.length; \
  parse_depth_--;                              \
  DebugParsing(debug)

// TODO(zq7): Make debug drawing looks more beautiful.
void Compiler::DebugParsing(const DebugParser& debug) {
  string prefix(debug.parse_depth * 3, '-');
  CHECK(debug.enter_pos < debug.exit_pos);
  string source = scanner_->GetSource(debug.enter_pos, debug.exit_pos);
  LOG(INFO) << prefix << std::to_string(debug.parse_depth) << "|   " << source;
}

void Compiler::ParseString() {
  DEBUG_PARSER_ENTER("string");

  string str = scanner_->GetSource(prev_.start, prev_.start + prev_.length);
  EmitConstant(Value(new ObjString(str)));

  DEBUG_PARSER_EXIT();
}

string Compiler::GetLexeme(Token tt) { return scanner_->GetLexeme(tt); }
void Compiler::ParseNumber() {
  DEBUG_PARSER_ENTER("number");

  double val = strtod(GetLexeme(prev_).c_str(), nullptr);
  EmitConstant(Value(val));

  DEBUG_PARSER_EXIT();
}

void Compiler::ParseExpression() {
  DEBUG_PARSER_ENTER("express");

  ParseWithPrecedenceOrder(PREC_ASSIGNMENT);

  DEBUG_PARSER_EXIT();
}

void Compiler::ParseGrouping() {
  DEBUG_PARSER_ENTER("group");

  ParseExpression();
  Consume(TOKEN_RIGHT_PAREN, "Expect ')' after ParseExpression.");

  DEBUG_PARSER_EXIT();
}

void Compiler::ParseUnary() {
  DEBUG_PARSER_ENTER("unary");

  TokenType op_type = prev_.type;
  ParseWithPrecedenceOrder(PREC_UNARY);
  switch (op_type) {
    case TOKEN_MINUS:
      EmitByte(OP_NEGATE);
      break;
    case TOKEN_BANG:
      EmitByte(OP_NOT);
      break;
    default:
      CHECK(false) << "Unreachable.";
      break;
  }

  DEBUG_PARSER_EXIT();
}

void Compiler::ParseBinary() {
  DEBUG_PARSER_ENTER("binary");

  TokenType op_type = prev_.type;
  PrecedenceRule* rule = GetRule(op_type);
  ParseWithPrecedenceOrder(PrecOrder(rule->prec_order + 1));
  switch (op_type) {
    case TOKEN_PLUS:
      EmitByte(OP_ADD);
      break;
    case TOKEN_MINUS:
      EmitByte(OP_SUB);
      break;
    case TOKEN_STAR:
      EmitByte(OP_MUL);
      break;
    case TOKEN_SLASH:
      EmitByte(OP_DIV);
      break;
    case TOKEN_BANG_EQUAL:
      EmitByte(OP_EQUAL, OP_NOT);
      break;
    case TOKEN_EQUAL_EQUAL:
      EmitByte(OP_EQUAL);
      break;
    case TOKEN_GREATER:
      EmitByte(OP_GREATER);
      break;
    case TOKEN_GREATER_EQUAL:
      EmitByte(OP_LESS, OP_NOT);
      break;
    case TOKEN_LESS:
      EmitByte(OP_LESS);
      break;
    case TOKEN_LESS_EQUAL:
      EmitByte(OP_GREATER, OP_NOT);
      break;
    default:
      CHECK(false) << "Unrecogined token: " << op_type;
      break;
  }

  DEBUG_PARSER_EXIT();
}

void Compiler::ParseLiteral() {
  DEBUG_PARSER_ENTER("literal");

  switch (prev_.type) {
    case TOKEN_FALSE:
      EmitByte(OP_FALSE);
      break;
    case TOKEN_TRUE:
      EmitByte(OP_TRUE);
      break;
    case TOKEN_NIL:
      EmitByte(OP_NIL);
      break;
    default:
      CHECK(false) << "Unreachable.";
      break;
  }

  DEBUG_PARSER_EXIT();
}

void Compiler::ParseWithPrecedenceOrder(PrecOrder prec_order) {
  DEBUG_PARSER_ENTER("prec_order");

  Advance();

  auto prev_rule = GetRule(prev_.type);
  CHECK(prev_rule);
  ParseFunc prefix_rule = prev_rule->prefix_rule;
  CHECK(prefix_rule);

  prefix_rule(this);

  while (GetRule(curr_.type)->prec_order >= prec_order) {
    Advance();
    ParseFunc infix_rule = GetRule(prev_.type)->infix_rule;
    infix_rule(this);
  }

  DEBUG_PARSER_EXIT();
}

}  // namespace xyxy
