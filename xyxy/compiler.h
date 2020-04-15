#ifndef XYXY_PARSE_H_
#define XYXY_PARSE_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "xyxy/chunk.h"
#include "xyxy/scanner.h"
#include "xyxy/type.h"

namespace xyxy {

typedef enum {
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
} PrecOrder;

class Compiler;

typedef std::function<void(Compiler*, bool can_assign)> ParseFunc;

typedef struct {
  ParseFunc prefix_rule;
  ParseFunc infix_rule;
  PrecOrder prec_order;
} PrecedenceRule;

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

  void ParseDeclaration();
  void ParseVarDeclaration();
  void ParseVariable(bool can_assign);
  void NamedVariable(bool can_assign);
  void ParseStatement();
  void ParsePrintStatement();
  void ParseExpressStatement();
  void ParseNumber(bool can_assign);
  void ParseGrouping(bool can_assign);
  void ParseUnary(bool can_assign);
  void ParseBinary(bool can_assign);
  void ParseExpression(bool can_assign);
  void ParseLiteral(bool can_assign);
  void ParseString(bool can_assign);
  void ParseBlock();
  void BeginScope();
  void EndScope();
  void DeclareLocals();
  bool ResolveLocal(const std::string& name, uint8* arg);
  uint8 HandleVariable(const string& msg);

  // Continue parsing until read a token that has a higher precedence.
  void ParseWithPrecedenceOrder(PrecOrder prec_order);

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

  // Local variable defition.
  struct LocalDef {
    Token token;
    int depth;
    string name;
  };

  static const int kLocalUnitialized;

 private:
  std::vector<LocalDef> locals_;
  int scope_depth_ = 0;
  Token curr_;
  Token prev_;
  std::unique_ptr<Chunk> chunk_;
  std::unique_ptr<Scanner> scanner_;
  // bool has_error_ = false;
  // bool panic_mode_ = false;
};
}  // namespace xyxy

#endif  // XYXY_COMPILER_H_
