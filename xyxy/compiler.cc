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
        {TOKEN_CONTINUE, CreateRule(nullptr, nullptr, PREC_NONE)},
        {TOKEN_BREAK, CreateRule(nullptr, nullptr, PREC_NONE)},
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
  scopes_.push_back(Scope());
}

Compiler::Compiler(const string& source) {
  scanner_ = std::make_unique<Scanner>(source);
  curr_ = Token{TOKEN_NONE, 0, 0, 0};
  prev_ = Token{TOKEN_NONE, 0, 0, 0};
  chunk_ = std::make_unique<Chunk>();
  scopes_.push_back(Scope());
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
  LOGccc << "Emiting OP_RETURN";
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
  LOGccc << "Emiting OP_CONSTANT: " << val.ToString();
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
  LOGccc << "Emiting OP_POP";
  EmitByte(OP_POP);
  ParseUntilHigherOrder(PREC_AND);
  PatchJump(end_jump);
}

void Compiler::LogicOr(bool can_assign) {
  int else_jump = EmitJump(OP_JUMP_IF_FALSE);
  // If current condition is true, then skip all the rest condition checks.
  int end_jump = EmitJump(OP_JUMP);
  PatchJump(else_jump);
  LOGccc << "Emiting OP_POP";
  EmitByte(OP_POP);

  ParseUntilHigherOrder(PREC_OR);
  PatchJump(end_jump);
}

void Compiler::ParseNumber(bool can_assign) {
  double val = strtod(GetLexeme(prev_).c_str(), nullptr);
  EmitConstant(Value(val));
}

void Compiler::ParseExpression() { ParseUntilHigherOrder(PREC_ASSIGNMENT); }

void Compiler::ParseGrouping(bool can_assign) {
  ParseExpression();
  Consume(TOKEN_RIGHT_PAREN, "Expect ')' after ParseExpression.");
}

void Compiler::ParseUnary(bool can_assign) {
  TokenType op_type = prev_.type;
  ParseUntilHigherOrder(PREC_UNARY);
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
  ParseUntilHigherOrder(PrecOrder(rule.prec_order + 1));
  switch (op_type) {
    case TOKEN_PLUS:
      LOGccc << "Emiting OP_ADD";
      EmitByte(OP_ADD);
      break;
    case TOKEN_MINUS:
      LOGccc << "Emiting OP_SUB";
      EmitByte(OP_SUB);
      break;
    case TOKEN_STAR:
      LOGccc << "Emiting OP_MUL";
      EmitByte(OP_MUL);
      break;
    case TOKEN_SLASH:
      LOGccc << "Emiting OP_SLAH";
      EmitByte(OP_DIV);
      break;
    case TOKEN_BANG_EQUAL:
      LOGccc << "Emiting OP_EQUAL OP_NOT";
      EmitByte(OP_EQUAL, OP_NOT);
      break;
    case TOKEN_EQUAL_EQUAL:
      LOGccc << "Emiting OP_EQUAL";
      EmitByte(OP_EQUAL);
      break;
    case TOKEN_GREATER:
      LOGccc << "Emiting OP_GREATER";
      EmitByte(OP_GREATER);
      break;
    case TOKEN_GREATER_EQUAL:
      LOGccc << "Emiting OP_LESS OP_NOT";
      EmitByte(OP_LESS, OP_NOT);
      break;
    case TOKEN_LESS:
      LOGccc << "Emiting OP_LESS";
      EmitByte(OP_LESS);
      break;
    case TOKEN_LESS_EQUAL:
      LOGccc << "Emiting OP_GREATER OP_NOT";
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
      LOGccc << "Emiting OP_FALSE";
      EmitByte(OP_FALSE);
      break;
    case TOKEN_TRUE:
      LOGccc << "Emiting OP_TRUE";
      EmitByte(OP_TRUE);
      break;
    case TOKEN_NIL:
      LOGccc << "Emiting OP_NIL";
      EmitByte(OP_NIL);
      break;
    default:
      CHECK(false) << "Unreachable.";
      break;
  }
}

void Compiler::ParseUntilHigherOrder(PrecOrder order) {
  Advance();

  LOGvvv << "Parsing until a higher order: " << GetLexeme(curr_) << " "
         << GetLexeme(prev_);

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
  LOGccc << "New local variable: " << GetLexeme(prev_)
         << " slot: " << std::to_string(locals_.size());
  if (!scopes_[scope_depth_].met_break_stmt) {
    scopes_[scope_depth_].owned_stack_num++;
  }
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
    LOGccc << "Initializing local variable: " << locals_.back().name;
    locals_.back().depth = scope_depth_;
  }
  else {
    // This is a global variable.
    LOGccc << "Emiting OP_DEFINE_GLOBAL " << global;
    EmitByte(OP_DEFINE_GLOBAL, global);
  }
}

bool Compiler::ResolveLocal(const std::string& name, uint8* arg) {
  for (size_t i = locals_.size(); i >= 1; i--) {
    if (locals_[i - 1].name == name) {
      if (locals_[i - 1].depth == kLocalUnitialized) {
        CHECK(false) << "Cannot read local variable in its own initializer.";
      }
      LOGccc << "Resolving local: " << name;
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
    LOGccc << "Emiting "
           << (set_op == OP_SET_LOCAL ? "OP_SET_LOCAL" : "OP_SET_GLOBAL") << " "
           << name;
    EmitByte(set_op, arg);
  }
  else {
    LOGccc << "Emiting "
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
  LOGvvv << "Parsing statement with curr token: " << GetLexeme(curr_);

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
    BeginScope(SCOPE_BLOCK);
    ParseBlock();
    EndScope();
  }
  else if (Match(TOKEN_CONTINUE)) {
    ParseContinueStmt();
  }
  else if (Match(TOKEN_BREAK)) {
    ParseBreakStmt();
  }
  else {
    ParseExpressStmt();
  }
}

void Compiler::ParseContinueStmt() {
  Consume(TOKEN_SEMICOLON, "Expect `;` after a break stmt");
  LOGvvv << "Parsing break stmt...";
  if (scope_depth_ == 0) {
    CHECK(false);
  }

  int for_scope = -1;
  int totol_stack_num = 0;
  for (int i = scope_depth_; i >= 0; i--) {
    if (scopes_[i].type == SCOPE_FOR) {
      for_scope = i;
      break;
    }
    LOGccc << "Scope " << i << " has " << scopes_[i].owned_stack_num
           << " values left on stack.";
    totol_stack_num += scopes_[i].owned_stack_num;
  }

  CHECK(for_scope != -1) << "Continue can't find for stmt.";
  for (int i = 0; i < totol_stack_num; i++) {
    LOGccc << "Emiting OP_POP to remove locals after continue";
    EmitByte(OP_POP);
  }

  int pc = GetChunk()->size();
  LOGccc << "Emiting OP_LOOP by continue at pc " << pc << " go back to "
         << scopes_[for_scope].loop_start;
  EmitLoop(scopes_[for_scope].loop_start);
}

void Compiler::ParseBreakStmt() {
  Consume(TOKEN_SEMICOLON, "Expect `;` after a break stmt");

  LOGvvv << "Parsing break stmt...";
  if (scope_depth_ == 0) {
    CHECK(false);
  }

  scopes_[scope_depth_].met_break_stmt = true;

  int for_scope = -1;
  int totol_stack_num = 0;
  for (int i = scope_depth_; i >= 0; i--) {
    LOGccc << "Scope " << i << " has " << scopes_[i].owned_stack_num
           << " values left on stack.";
    totol_stack_num += scopes_[i].owned_stack_num;
    if (scopes_[i].type == SCOPE_FOR) {
      for_scope = i;
      break;
    }
  }

  CHECK(for_scope != -1) << "Break can't find for stmt.";
  for (int i = 0; i < totol_stack_num; i++) {
    LOGccc << "Emiting OP_POP to remove locals after break";
    EmitByte(OP_POP);
  }

  // Emit OP_JUMP, will refill how long it jumps.
  int break_jump = EmitJump(OP_JUMP);
  LOGccc << "Emiting OP_JUMP to break execution at pc: " << break_jump - 1;
  scopes_[for_scope].breaks.push_back(break_jump);
}

void Compiler::ParseForStmt() {
  LOGvvv << "Parsing for statement...";

  // Treat for as a new scope since it might define new varibles.
  BeginScope(SCOPE_FOR);

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
  scopes_[scope_depth_].loop_start = loop_start;

  int exit_jump = -1;
  if (!Match(TOKEN_SEMICOLON)) {
    ParseExpression();
    Consume(TOKEN_SEMICOLON, "Expect ';' after loop condition");

    int pc = GetChunk()->size();
    LOGccc << "Emiting OO_JUMP_IF_FALSE at pc " << pc;
    exit_jump = EmitJump(OP_JUMP_IF_FALSE);
    LOGccc << "Emiting OP_POP to pop the for condition";
    EmitByte(OP_POP);
  }

  // Parse the increment statement.
  if (!Match(TOKEN_RIGHT_PAREN)) {
    int inc_jump = EmitJump(OP_JUMP);

    int inc_start = GetChunk()->size();
    LOGvvv << "Increment start: " << inc_start;
    ParseExpression();
    LOGccc << "Emiting OP_POP to pop the expression value";
    EmitByte(OP_POP);
    Consume(TOKEN_RIGHT_PAREN, "Expect ')' after finishing 'for'.");

    LOGccc << "Emiting OP_LOOP by increment stmt";
    EmitLoop(loop_start);
    loop_start = inc_start;
    scopes_[scope_depth_].loop_start = loop_start;
    PatchJump(inc_jump);
  }

  // Parse the main body of the for statement.
  ParseStmt();

  LOGccc << "Emiting OP_LOOP by for main body";
  EmitLoop(loop_start);

  // Finish the jump if the condition becomes false.
  if (exit_jump != -1) {
    PatchJump(exit_jump);
    // Pop out the condition value left on the stack.
    LOGccc << "Emiting OP_POP to pop the for condition";
    EmitByte(OP_POP);
  }

  EndScope();
}

void Compiler::EmitLoop(int loop_start) {
  EmitByte(OP_LOOP);

  int offset = GetChunk()->size() - loop_start - 1;
  if (offset > UINT16_MAX) {
    CHECK(false) << "Loop body too large.";
  }

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

  LOGccc << "Emiting OP_POP";
  EmitByte(OP_POP);
  ParseStmt();

  LOGccc << "Emiting OP_LOOP by while main body";
  EmitLoop(loop_start);

  PatchJump(exit_jump);
  LOGccc << "Emiting OP_POP to pop while condition";
  // Pop the condition value on the stack.
  EmitByte(OP_POP);
}

void Compiler::PatchJump(int patch_place) {
  int jump_count = GetChunk()->size() - patch_place - 2;
  CHECK(jump_count >= 0);

  if (jump_count > UINT16_MAX) {
    CHECK(false) << "Too much code to jump over.";
  }

  LOGvvv << "Patching at address " << patch_place << " to jump over "
         << jump_count;
  GetChunk()->WriteAt(patch_place, (jump_count >> 8) & 0xff);
  GetChunk()->WriteAt(patch_place + 1, jump_count & 0xff);
}

void Compiler::ParseIfStmt() {
  LOGvvv << "Parsing if statement...";
  Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if' ");
  ParseExpression();
  Consume(TOKEN_RIGHT_PAREN, "Expect ')' after 'if' condition");

  int then_branch = EmitJump(OP_JUMP_IF_FALSE);
  LOGccc << "Emiting OP_POP to pop if condition";
  EmitByte(OP_POP);
  ParseStmt();
  int else_branch = EmitJump(OP_JUMP);
  PatchJump(then_branch);

  LOGccc << "Emiting OP_POP to pop if condition";
  EmitByte(OP_POP);
  if (Match(TOKEN_ELSE)) {
    ParseStmt();
  }
  PatchJump(else_branch);
}

std::string DebugScope(Scope sp) {
  std::string ret;
  ret += "Scope";
  ret += std::to_string(sp.depth);
  ret += (sp.type == SCOPE_FOR ? "For" : "");
  ret += "{";
  ret += std::to_string(sp.start_ln);
  ret += ", ";
  ret += std::to_string(sp.end_ln);
  ret += ", ";
  ret += std::to_string(sp.start_pc);
  ret += ", ";
  ret += std::to_string(sp.end_pc);
  ret += ", ";
  ret += ", ";
  ret += std::to_string(sp.owned_stack_num);
  ret += "}";
  return ret;
}

void Compiler::BeginScope(ScopeType type) {
  scope_depth_ = scopes_.size();
  // Don't know the end_pc yet, will refill after we know it.
  scopes_.push_back(Scope(type,
                          /*start_pc=*/GetChunk()->size(),
                          /*start_ln=*/prev_.line,
                          /*depth=*/scope_depth_));

  LOGrrr << "Entering scope: " << DebugScope(scopes_[scope_depth_]);
}

void Compiler::EndScope() {
  // Refill the end_pc for every scope.
  auto& scope = scopes_[scope_depth_];

  scope.end_pc = GetChunk()->size();
  scope.end_ln = prev_.line;

  LOGrrr << "Exiting scope: " << DebugScope(scope);

  // Remove all the local varibles that are out of this scope.
  while (!locals_.empty() && locals_.back().depth >= scope_depth_) {
    LOGccc << "Emiting OP_POP to remove local varibles..."
           << GetLexeme(locals_.back().token);
    EmitByte(OP_POP);
    locals_.pop_back();
  }

  if (scope.type == SCOPE_FOR) {
    for (auto break_jump : scope.breaks) {
      PatchJump(break_jump);
      scope.breaks.pop_back();
    }
  }

  scopes_.pop_back();
  scope_depth_--;
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

  LOGccc << "Emiting OP_PRINT";
  EmitByte(OP_PRINT);
}

void Compiler::ParseExpressStmt() {
  LOGvvv << "Parsing expression statement...";

  ParseExpression();
  Consume(TOKEN_SEMICOLON, "Expect ';' after an expression.");

  LOGccc << "Emiting OP_POP to pop the expression value";
  EmitByte(OP_POP);
}

}  // namespace xyxy
