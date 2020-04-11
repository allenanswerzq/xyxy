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

typedef std::function<void(Compiler*)> ParseFunc;

typedef struct {
  ParseFunc prefix_rule;
  ParseFunc infix_rule;
  PrecOrder prec_order;
} PrecedenceRule;

class Compiler {
 public:
  Compiler() {
    scanner_ = std::make_unique<Scanner>();
    curr_ = Token{TOKEN_NONE, 0, 0, 0};
    prev_ = Token{TOKEN_NONE, 0, 0, 0};
    chunk_ = std::make_unique<Chunk>();
  }

  virtual ~Compiler() = default;

  void Compile(const string& source_code) {
    VLOG(1) << "Compiling: " << source_code;
    scanner_->SetSource(source_code);
    curr_ = Token{TOKEN_NONE, 0, 0, 0};
    prev_ = Token{TOKEN_NONE, 0, 0, 0};
    Advance();
    while (!Match(TOKEN_EOF)) {
      ParseDeclaration();
    }
  }

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
  void ParseVariable();
  void ParseStatement();
  void ParsePrintStatement();
  void ParseExpressStatement();
  void ParseNumber();
  void ParseGrouping();
  void ParseUnary();
  void ParseBinary();
  void ParseExpression();
  void ParseLiteral();
  void ParseString();
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

  struct DebugParser {
    int parse_depth = 0;
    int enter_pos;
    int exit_pos;
    string msg;
  };

  void DebugParsing(const DebugParser& debug);

 private:
  Token curr_;
  Token prev_;
  std::unique_ptr<Chunk> chunk_;
  std::unique_ptr<Scanner> scanner_;
  bool has_error_ = false;
  bool panic_mode_ = false;
  int parse_depth_ = 1;
};
}  // namespace xyxy

#endif  // XYXY_COMPILER_H_
