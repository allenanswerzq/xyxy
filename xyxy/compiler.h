#ifndef XYXY_PARSE_H_
#define XYXY_PARSE_H_

#include <functional>
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
  TokenType token_type;
  ParseFunc prefix_rule;
  ParseFunc infix_rule;
  PrecOrder prec_order;
} PrecedenceRule;

static std::vector<PrecedenceRule*>* GlobalPrecedenceRules() {
  static std::vector<PrecedenceRule*>* registry;
  if (!registry) {
    registry = new std::vector<PrecedenceRule*>;
  }
  return registry;
}

class Compiler {
 public:
  Compiler(const string& source, std::shared_ptr<Chunk> chunk) {
    scanner_ = std::make_unique<Scanner>(source);
    chunk_ = chunk;
    curr_ = Token{TOKEN_NONE, 0, 0, 0};
    prev_ = Token{TOKEN_NONE, 0, 0, 0};
  }

  virtual ~Compiler() = default;

  void advance();
  void consume(TokenType type, const string& msg);

  // Add a Value `val` into chunk and return its index.
  int make_constant(Value val);

  // Emit a {OP_CONSTANT idx} inst.
  // Note: idx specifies where the constant stored inside chunk's value area.
  void emit_constant(Value val);

  void emit_byte(uint8 byte);
  void emit_byte(uint8 byte1, uint8 byte2);
  void emit_return();

  void parse_number();
  void parse_grouping();
  void parse_unary();
  void parse_binary();
  void parse_expression();
  void parse_literal();
  void parse_string();
  void parse_with_prec_order(PrecOrder prec_order);

  string get_lexeme(Token tt);

  std::shared_ptr<Chunk> GetChunk() { return chunk_; }

  Token prev_token() { return prev_; }
  Token curr_token() { return curr_; }

  PrecedenceRule* get_rule(TokenType type) {
    CHECK(GlobalPrecedenceRules());
    return GlobalPrecedenceRules()->at(type);
  }

  struct DebugParser {
    int parse_depth = 0;
    int enter_pos;
    int exit_pos;
    string msg;
  };

  void debug_parser(const DebugParser& debug);

 private:
  Token curr_;
  Token prev_;
  std::shared_ptr<Chunk> chunk_;
  std::unique_ptr<Scanner> scanner_;
  bool has_error_ = false;
  bool panic_mode_ = false;
  int parse_depth_ = 1;
};
}  // namespace xyxy

namespace register_prec_rule {

struct PrecRuleDefWrapper {
  PrecRuleDefWrapper(::xyxy::TokenType type) {
    rule = new ::xyxy::PrecedenceRule();
    rule->token_type = type;
    ::xyxy::GlobalPrecedenceRules()->push_back(rule);
  }

  PrecRuleDefWrapper& Prefix_Rule(::xyxy::ParseFunc f) {
    rule->prefix_rule = f;
    return *this;
  }

  PrecRuleDefWrapper& Infix_Rule(::xyxy::ParseFunc f) {
    rule->infix_rule = f;
    return *this;
  }

  PrecRuleDefWrapper& Prec_Order(::xyxy::PrecOrder order) {
    rule->prec_order = order;
    return *this;
  }

 private:
  ::xyxy::PrecedenceRule* rule;
};

}  // namespace register_prec_rule

#define REGISTER_PREC_RULE(name) REGISTER_PREC_RULE_HELPER(__COUNTER__, name)
#define REGISTER_PREC_RULE_HELPER(ctr, name) REGISTER_PREC_RULE_UNIQ(ctr, name)
#define REGISTER_PREC_RULE_UNIQ(ctr, name)                           \
  static ::register_prec_rule::PrecRuleDefWrapper register_inst##ctr \
      XY_ATTRIBUTE_UNUSED = ::register_prec_rule::PrecRuleDefWrapper(name)

#endif  // XYXY_COMPILER_H_
