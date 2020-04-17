#ifndef XYXY_PARSE_H_
#define XYXY_PARSE_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "xyxy/chunk.h"
#include "xyxy/scanner.h"
#include "xyxy/type.h"

namespace xyxy {

enum PrecOrder {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
};

class Compiler;

typedef std::function<void(Compiler*, bool can_assign)> ParseFunc;

struct PrecedenceRule {
  ParseFunc prefix_rule;
  ParseFunc infix_rule;
  PrecOrder prec_order;
};

enum ScopeType {
  SCOPE_UNDEF,
  SCOPE_BLOCK,
  SCOPE_FOR,
  SCOPE_FUNC,
  SCOPE_CLASS,
  SCOPE_MAIN
};

// TODO(): may consider write this as a class.
struct Scope {
  Scope() {}
  explicit Scope(ScopeType tp, int pc, int ln, int dp)
      : type(tp), start_pc(pc), start_ln(ln), depth(dp) {}
  ScopeType type;
  int start_pc = 0;
  int end_pc = 0;
  int start_ln = 0;
  int end_ln = 0;
  int depth = 0;
  int loop_start = 0;
  int owned_stack_num = 0;
  bool met_break_stmt = false;
  std::vector<int> breaks;
};

std::string DebugScope(Scope sp);

class Compiler {
 public:
  Compiler();
  explicit Compiler(const string& source);
  virtual ~Compiler() = default;

  void Compile(const string& source_code);

  void Advance();
  bool Match(TokenType type);
  bool CheckType(TokenType type);
  void Consume(TokenType type, const string& msg);

  void DefineVariable(uint8 global);

  // Add a Value `val` into chunk and return its index.
  int MakeConstant(Value val);
  uint8 IdentifierConstant(const string& name);

  // Emit a {OP_CONSTANT idx} inst.
  // Note: idx specifies where the constant stored inside chunk's value area.
  void EmitConstant(Value val);

  void EmitByte(uint8 byte);
  void EmitByte(uint8 byte1, uint8 byte2);
  void EmitReturn();
  int EmitJump(uint8 inst);
  void PatchJump(int place);
  void EmitLoop(int loop_start);

  void ParseDeclaration();
  void ParseVarDeclaration();
  void ParseVariable(bool can_assign);
  void NamedVariable(bool can_assign);
  void ParseStmt();
  void ParsePrintStmt();
  void ParseExpressStmt();
  void ParseNumber(bool can_assign);
  void ParseGrouping(bool can_assign);
  void ParseUnary(bool can_assign);
  void ParseBinary(bool can_assign);
  void ParseLiteral(bool can_assign);
  void ParseString(bool can_assign);
  void ParseExpression();
  void LogicAnd(bool can_assign);
  void LogicOr(bool can_assign);

  void ParseBlock();
  void BeginScope(ScopeType type);
  void EndScope();
  void DeclareLocals();
  bool ResolveLocal(const std::string& name, uint8* arg);
  uint8 HandleVariable(const string& msg);

  void ParseIfStmt();
  void ParseForStmt();
  void ParseWhileStmt();
  void ParseContinueStmt();
  void ParseBreakStmt();

  // Continue parsing until read a token that has a higher precedence.
  void ParseUntilHigherOrder(PrecOrder prec_order);

  string GetLexeme(Token tt);

  Chunk* GetChunk() { return chunk_.get(); }

  Token PrevToken() { return prev_; }
  Token CurrToken() { return curr_; }

  static const std::unordered_map<int, PrecedenceRule>& kPrecedenceTable;

  PrecedenceRule GetRule(TokenType type) {
    auto it = kPrecedenceTable.find(type);
    CHECK(it != kPrecedenceTable.end());
    return it->second;
  }

  // Local variable definition.
  struct LocalDef {
    Token token;
    int depth;
    string name;
  };

  static const int kLocalUnitialized;

 private:
  Token curr_;
  Token prev_;
  std::unique_ptr<Chunk> chunk_;
  std::unique_ptr<Scanner> scanner_;
  int scope_depth_ = 0;
  std::vector<Scope> scopes_;
  std::vector<LocalDef> locals_;
  // bool has_error_ = false;
  // bool panic_mode_ = false;
};
}  // namespace xyxy

#endif  // XYXY_COMPILER_H_
