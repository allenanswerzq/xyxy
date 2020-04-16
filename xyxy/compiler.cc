#include "xyxy/compiler.h"

#include "xyxy/logging.h"
#include "xyxy/object.h"
#include "xyxy/scanner.h"
#include "xyxy/vm.h"

namespace xyxy {

static PrecedenceRule CreateRule(ParseFunc prefix, ParseFunc infix,
                                 PrecOrder order) {
  return PrecedenceRule{prefix, infix, order};
}

const std::unordered_map<int, PrecedenceRule>& Compiler::kPrecedenceTable =
    *new std::unordered_map<int, PrecedenceRule>({
        {TOKEN_LEFT_PAREN,
         CreateRule(&Compiler::ParseGrouping, nullptr, PREC_NONE)},
        {TOKEN_RIGHT_PAREN, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_LEFT_BRACE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_RIGHT_BRACE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_COMMA, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_DOT, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_MINUS,
         CreateRule(&Compiler::ParseUnary, &Compiler::ParseBinary, PREC_TERM)},
        {TOKEN_PLUS, CreateRule(nullptr, &Compiler::ParseBinary, PREC_TERM)},
        {TOKEN_SEMICOLON, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_SLASH, CreateRule(nullptr, &Compiler::ParseBinary, PREC_FACTOR)},
        {TOKEN_STAR, CreateRule(nullptr, &Compiler::ParseBinary, PREC_FACTOR)},
        {TOKEN_BANG, CreateRule(&Compiler::ParseUnary, nullptr, PREC_NONE)},
        {TOKEN_BANG_EQUAL,
         CreateRule(nullptr, &Compiler::ParseBinary, PREC_EQUALITY)},
        {TOKEN_EQUAL, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_EQUAL_EQUAL,
         CreateRule(nullptr, &Compiler::ParseBinary, PREC_EQUALITY)},
        {TOKEN_GREATER,
         CreateRule(nullptr, &Compiler::ParseBinary, PREC_COMPARISON)},
        {TOKEN_GREATER_EQUAL,
         CreateRule(nullptr, &Compiler::ParseBinary, PREC_COMPARISON)},
        {TOKEN_LESS,
         CreateRule(nullptr, &Compiler::ParseBinary, PREC_COMPARISON)},
        {TOKEN_LESS_EQUAL,
         CreateRule(nullptr, &Compiler::ParseBinary, PREC_COMPARISON)},
        {TOKEN_IDENTIFIER,
         CreateRule(&Compiler::ParseVariable, nullptr, PREC_NONE)},
        {TOKEN_STRING, CreateRule(&Compiler::ParseString, nullptr, PREC_NONE)},
        {TOKEN_NUMBER, CreateRule(&Compiler::ParseNumber, nullptr, PREC_NONE)},
        {TOKEN_AND, CreateRule(nullptr, &Compiler::LogicAnd, PREC_AND)},
        {TOKEN_IF, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_ELSE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_FALSE, CreateRule(&Compiler::ParseLiteral, nullptr, PREC_NONE)},
        {TOKEN_FUN, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_FOR, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_NIL, CreateRule(&Compiler::ParseLiteral, nullptr, PREC_NONE)},
        {TOKEN_OR, CreateRule(nullptr, &Compiler::LogicOr, PREC_OR)},
        {TOKEN_CLASS, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_PRINT, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_RETURN, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_SUPER, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_THIS, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_TRUE, CreateRule(&Compiler::ParseLiteral, nullptr, PREC_NONE)},
        {TOKEN_VAR, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_WHILE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_NEWLINE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_WHITESPACE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_ERROR, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_EOF, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_EOF, CreateRule(&Compiler::ParseGrouping, nullptr, PREC_NONE)},
    });

const int Compiler::kLocalUnitialized = -1;

Compiler::Compiler() {
  scanner_ = std::make_unique<Scanner>();
  curr_ = Token{TOKEN_NONE, 0, 0, 0};
  prev_ = Token{TOKEN_NONE, 0, 0, 0};
  chunk_ = std::make_unique<Chunk>();
}

Compiler::Compiler(const string& source) {
  scanner_ = std::make_unique<Scanner>(source);
  curr_ = Token{TOKEN_NONE, 0, 0, 0};
  prev_ = Token{TOKEN_NONE, 0, 0, 0};
  chunk_ = std::make_unique<Chunk>();
}

void Compiler::Compile(const string& source_code) {
  LOGvvv << "Compiling: " << source_code;
  scanner_->SetSource(source_code);
  curr_ = Token{TOKEN_NONE, 0, 0, 0};
  prev_ = Token{TOKEN_NONE, 0, 0, 0};
  Advance();
  while (!Match(TOKEN_EOF)) {
    ParseDeclaration();
  }
}

void Compiler::Advance() {
  prev_ = curr_;
  while (true) {
    curr_ = scanner_->ScanToken();
    LOGvvv << "ScanToken: | Curr: " << GetLexeme(curr_)
           << " | Prev: " << GetLexeme(prev_);
    if (curr_.type != TOKEN_ERROR) {
      break;
    }
  }
}

void Compiler::Consume(TokenType type, const string& msg) {
  CHECK(curr_.type == type) << msg;
  Advance();
  return;
}

void Compiler::EmitByte(uint8 byte) { GetChunk()->Write(byte, prev_.line); }

void Compiler::EmitReturn() {
  LOGrrr << "Emiting OP_RETURN";
  EmitByte(OP_RETURN);
}

void Compiler::EmitByte(uint8 byte1, uint8 byte2) {
  EmitByte(byte1);
  EmitByte(byte2);
}

int Compiler::EmitJump(uint8 inst) {
  EmitByte(inst);
  EmitByte(0);
  EmitByte(0);
  return GetChunk()->size() - 2;
}

int Compiler::MakeConstant(Value val) {
  // Get the index after adding this val into chunk.
  int idx = GetChunk()->AddConstant(val);
  CHECK(idx <= UINT8_MAX) << "Not expect to many consts in one chunk.";
  return idx;
}

void Compiler::EmitConstant(Value val) {
  LOGrrr << "Emiting OP_CONSTANT: " << val.ToString();
  EmitByte(OP_CONSTANT, MakeConstant(val));
}

void Compiler::ParseString(bool can_assign) {
  string str = scanner_->GetSource(prev_.start, prev_.start + prev_.length);
  int n = str.size();
  CHECK(n >= 2);
  EmitConstant(Value(new ObjString(str.substr(1, n - 2))));
}

string Compiler::GetLexeme(Token tt) { return scanner_->GetLexeme(tt); }

void Compiler::LogicAnd(bool can_assign) {
  int end_jump = EmitJump(OP_JUMP_IF_FALSE);
  LOGrrr << "Emiting OP_POP";
  EmitByte(OP_POP);
  ParseWithPrecedenceOrder(PREC_AND);
  PatchJump(end_jump);
}

void Compiler::LogicOr(bool can_assign) {
  int else_jump = EmitJump(OP_JUMP_IF_FALSE);
  // If current condition is true, then skip all the rest condition checks.
  int end_jump = EmitJump(OP_JUMP);
  PatchJump(else_jump);
  LOGrrr << "Emiting OP_POP";
  EmitByte(OP_POP);

  ParseWithPrecedenceOrder(PREC_OR);
  PatchJump(end_jump);
}

void Compiler::ParseNumber(bool can_assign) {
  double val = strtod(GetLexeme(prev_).c_str(), nullptr);
  EmitConstant(Value(val));
}

void Compiler::ParseExpression() { ParseWithPrecedenceOrder(PREC_ASSIGNMENT); }

void Compiler::ParseGrouping(bool can_assign) {
  ParseExpression();
  Consume(TOKEN_RIGHT_PAREN, "Expect ')' after ParseExpression.");
}

void Compiler::ParseUnary(bool can_assign) {
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
}

void Compiler::ParseBinary(bool can_assign) {
  TokenType op_type = prev_.type;
  PrecedenceRule rule = GetRule(op_type);
  ParseWithPrecedenceOrder(PrecOrder(rule.prec_order + 1));
  switch (op_type) {
    case TOKEN_PLUS:
      LOGrrr << "Emiting OP_ADD";
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
}

void Compiler::ParseLiteral(bool can_assign) {
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
}

void Compiler::ParseWithPrecedenceOrder(PrecOrder order) {
  Advance();

  auto rule = GetRule(prev_.type);
  ParseFunc prefix_rule = rule.prefix_rule;
  CHECK(prefix_rule);

  bool can_assign = order <= PREC_ASSIGNMENT;
  prefix_rule(this, can_assign);

  while (GetRule(curr_.type).prec_order >= order) {
    Advance();
    ParseFunc infix_rule = GetRule(prev_.type).infix_rule;
    infix_rule(this, can_assign);
  }

  // NOTE: further research on this.
  if (can_assign && Match(TOKEN_EQUAL)) {
    // TODO(): Error handling.
    CHECK(false) << "Invalid assignment target";
  }
}

bool Compiler::CheckType(TokenType type) { return curr_.type == type; }

bool Compiler::Match(TokenType type) {
  if (!CheckType(type)) {
    return false;
  }
  Advance();
  return true;
}

// declaration    -> varDecl
//                | statement ;
void Compiler::ParseDeclaration() {
  LOGvvv << "Parsing declaration...";

  if (Match(TOKEN_VAR)) {
    ParseVarDeclaration();
  }
  else {
    ParseStmt();
  }
}

uint8 Compiler::IdentifierConstant(const string& name) {
  return MakeConstant(Value(new ObjString(name)));
}

uint8 Compiler::HandleVariable(const string& msg) {
  Consume(TOKEN_IDENTIFIER, msg);

  DeclareLocals();
  if (scope_depth_ > 0) return 0;

  return IdentifierConstant(GetLexeme(prev_));
}

void Compiler::DeclareLocals() {
  // If we are at the global scope, do nothing.
  if (scope_depth_ == 0) return;

  if (locals_.size() >= UINT8_MAX) {
    CHECK(false) << "Too many local variables defined";
  }
  for (size_t i = locals_.size(); i >= 1; i--) {
    if (locals_[i - 1].depth != kLocalUnitialized &&
        locals_[i - 1].depth < scope_depth_) {
      break;
    }
    if (GetLexeme(prev_) == locals_[i - 1].name) {
      CHECK(false) << "Variable with the name `" << GetLexeme(prev_)
                   << "` already defined in this scope.";
    }
  }
  LOGrrr << "New local variable: " << GetLexeme(prev_)
         << " slot: " << std::to_string(locals_.size());
  locals_.push_back(LocalDef{prev_, kLocalUnitialized, GetLexeme(prev_)});
}

void Compiler::ParseVarDeclaration() {
  LOGvvv << "Parsing var declaration...";

  uint8 global = HandleVariable("Expect variable name");
  if (Match(TOKEN_EQUAL)) {
    ParseExpression();
  }
  else {
    // By default, assign a variable to NILL;
    EmitByte(OP_NIL);
  }
  Consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  DefineVariable(global);
}

void Compiler::DefineVariable(uint8 global) {
  if (scope_depth_ > 0) {
    // Mark the local variable as initialized.
    CHECK(!locals_.empty());
    LOGrrr << "Initializing local variable: " << locals_.back().name;
    locals_.back().depth = scope_depth_;
  }
  else {
    // This is a global variable.
    LOGrrr << "Emiting OP_DEFINE_GLOBAL " << global;
    EmitByte(OP_DEFINE_GLOBAL, global);
  }
}

bool Compiler::ResolveLocal(const std::string& name, uint8* arg) {
  for (size_t i = locals_.size(); i >= 1; i--) {
    if (locals_[i - 1].name == name) {
      if (locals_[i - 1].depth == kLocalUnitialized) {
        CHECK(false) << "Cannot read local variable in its own initializer.";
      }
      *arg = i - 1;
      return true;
    }
  }
  return false;
}

void Compiler::ParseVariable(bool can_assign) {
  LOGvvv << "Parsing variable...";
  NamedVariable(can_assign);
}

void Compiler::NamedVariable(bool can_assign) {
  uint8 arg = 0;
  uint8 set_op = 0;
  uint8 get_op = 0;
  std::string name = GetLexeme(prev_);
  if (ResolveLocal(name, &arg)) {
    // If the prev_ is a local variable.
    LOGvvv << "Find the local variable: " << name
           << " slot: " << std::to_string(arg);
    set_op = OP_SET_LOCAL;
    get_op = OP_GET_LOCAL;
  }
  else {
    arg = IdentifierConstant(name);
    set_op = OP_SET_GLOBAL;
    get_op = OP_GET_GLOBAL;
  }

  if (can_assign && Match(TOKEN_EQUAL)) {
    ParseExpression();
    LOGrrr << "Emiting "
           << (set_op == OP_SET_LOCAL ? "OP_SET_LOCAL" : "OP_SET_GLOBAL") << " "
           << name;
    EmitByte(set_op, arg);
  }
  else {
    LOGrrr << "Emiting "
           << (get_op == OP_GET_LOCAL ? "OP_GET_LOCAL" : "OP_GET_GLOBAL") << " "
           << name;
    EmitByte(get_op, arg);
  }
}

// statement     ->  exprStmt
//               |   printStmt
//               |   ifStmt
//               |   whileStmt
//               |   forStmt
//               |   block ;
// block         ->  "{" declaration* "}"
void Compiler::ParseStmt() {
  LOGvvv << "Parsing statement...";
  if (Match(TOKEN_PRINT)) {
    ParsePrintStmt();
  }
  else if (Match(TOKEN_IF) || Match(TOKEN_ELIF)) {
    ParseIfStmt();
  }
  else if (Match(TOKEN_WHILE)) {
    ParseWhileStmt();
  }
  else if (Match(TOKEN_FOR)) {
    ParseForStmt();
  }
  else if (Match(TOKEN_LEFT_BRACE)) {
    BeginScope();
    ParseBlock();
    EndScope();
  }
  else {
    ParseExpressStmt();
  }
}

void Compiler::ParseForStmt() {
  LOGvvv << "Parsing for statement...";

  // Treat for as a new scope since it might define new varibles.
  BeginScope();

  // Parse the initializer.
  Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  if (Match(TOKEN_SEMICOLON)) {
    // No initializer.
  }
  else if (Match(TOKEN_VAR)) {
    ParseVarDeclaration();
  }
  else {
    ParseExpressStmt();
  }

  // Parse the for condition.
  int loop_start = GetChunk()->size();
  int exit_jump = -1;
  if (!Match(TOKEN_SEMICOLON)) {
    ParseExpression();
    Consume(TOKEN_SEMICOLON, "Expect ';' after loop condition");

    exit_jump = EmitJump(OP_JUMP_IF_FALSE);
    EmitByte(OP_POP);
  }

  // Parse the increment statement.
  if (!Match(TOKEN_RIGHT_PAREN)) {
    int inc_jump = EmitJump(OP_JUMP);

    int inc_start = GetChunk()->size();
    LOGvvv << "Increment start: " << inc_start;
    ParseExpression();
    EmitByte(OP_POP);
    Consume(TOKEN_RIGHT_PAREN, "Expect ')' after finishing 'for'.");

    EmitLoop(loop_start);
    loop_start = inc_start;
    PatchJump(inc_jump);
  }

  // Parse the main body of the for statement.
  ParseStmt();

  EmitLoop(loop_start);

  EndScope();

  // Finish the jump if the condition becomes false.
  if (exit_jump != -1) {
    PatchJump(exit_jump);
    // Pop out the condition value left on the stack.
    EmitByte(OP_POP);
  }
}

void Compiler::EmitLoop(int loop_start) {
  EmitByte(OP_LOOP);

  int offset = GetChunk()->size() - loop_start - 1;
  if (offset > UINT16_MAX) {
    CHECK(false) << "Loop body too large.";
  }

  LOGrrr << "Emiting OP_LOOP"
         << " " << offset;
  LOGrrr << "Emiting OP_POP";

  EmitByte((offset >> 8) & 0xff);
  EmitByte(offset & 0xff);
}

void Compiler::ParseWhileStmt() {
  LOGvvv << "Parsing while statement...";
  int loop_start = GetChunk()->size();

  Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while' ");
  ParseExpression();
  Consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'while' condition");

  int exit_jump = EmitJump(OP_JUMP_IF_FALSE);

  LOGrrr << "Emiting OP_POP";
  EmitByte(OP_POP);
  ParseStmt();

  EmitLoop(loop_start);

  PatchJump(exit_jump);
  LOGrrr << "Emiting OP_POP";
  // Pop the condition value on the stack.
  EmitByte(OP_POP);
}

void Compiler::PatchJump(int patch_place) {
  int jump_count = GetChunk()->size() - patch_place - 2;
  CHECK(jump_count >= 0);

  if (jump_count > UINT16_MAX) {
    CHECK(false) << "Too much code to jump over.";
  }

  LOGrrr << "Jump over: " << jump_count;
  GetChunk()->WriteAt(patch_place, (jump_count >> 8) & 0xff);
  GetChunk()->WriteAt(patch_place + 1, jump_count & 0xff);
}

void Compiler::ParseIfStmt() {
  LOGvvv << "Parsing if statement...";
  Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if' ");
  ParseExpression();
  Consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'if' condition");

  int then_branch = EmitJump(OP_JUMP_IF_FALSE);
  LOGrrr << "Emiting OP_POP";
  EmitByte(OP_POP);
  ParseStmt();
  int else_branch = EmitJump(OP_JUMP);
  PatchJump(then_branch);

  LOGrrr << "Emiting OP_POP";
  EmitByte(OP_POP);
  if (Match(TOKEN_ELSE)) {
    ParseStmt();
  }
  PatchJump(else_branch);
}

void Compiler::BeginScope() { scope_depth_++; }

void Compiler::EndScope() {
  scope_depth_--;
  // Remove all the local varibles that are out of its scope.
  while (!locals_.empty() && locals_.back().depth > scope_depth_) {
    LOGrrr << "Emiting OP_POP";
    EmitByte(OP_POP);
    locals_.pop_back();
  }
}

void Compiler::ParseBlock() {
  LOGvvv << "Parsing {...";

  while (!CheckType(TOKEN_RIGHT_BRACE) && !CheckType(TOKEN_EOF)) {
    ParseDeclaration();
  }
  Consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

void Compiler::ParsePrintStmt() {
  LOGvvv << "Parsing print...";

  ParseExpression();
  Consume(TOKEN_SEMICOLON, "Expect ';' after a value.");

  LOGrrr << "Emiting OP_PRINT";
  EmitByte(OP_PRINT);
}

void Compiler::ParseExpressStmt() {
  LOGvvv << "Parsing expression statement...";

  ParseExpression();
  Consume(TOKEN_SEMICOLON, "Expect ';' after an expression.");

  LOGrrr << "Emiting OP_POP";
  EmitByte(OP_POP);
}

}  // namespace xyxy
