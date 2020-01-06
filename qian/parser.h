#ifndef QIAN_PARSE_H_
#define QIAN_PARSE_H_

#include <functional>

#include "chunk.h"
#include "debug.h"
#include "logging.h"
#include "vector.h"
#include "scanner.h"
#include "type.h"

namespace qian {

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

class Parser;

typedef std::function<void(Parser*)> ParseFunc;

typedef struct {
  TokenType token_type = TOKEN_NONE;
  ParseFunc prefix_rule = nullptr;
  ParseFunc infix_rule = nullptr;
  PrecOrder prec_order = PREC_NONE;
} PrecRule;

inline Vector<PrecRule*>* GlobalPrecRule() {
  static Vector<PrecRule*>* registry;
  if (!registry) {
    registry = new Vector<PrecRule*>;
  }
  return registry;
}

class Parser {
 public:
  Parser(const string& source, Chunk* chunk);
  virtual ~Parser();

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
  void parse_with_prec_order(PrecOrder prec_order);

  void debug_parse(const string& msg, int start, int end);

  string to_string(Token tk) {
    return scanner_->get_lexeme(tk);
  };

  Chunk* GetChunk() { return chunk_; }

  Token prev_token() { return prev_; }
  Token curr_token() { return curr_; }

  PrecRule* get_rule(TokenType type) {
    CHECK(GlobalPrecRule());
    return GlobalPrecRule()->Get(type);
  }

 private:
  Token curr_;
  Token prev_;
  Chunk* chunk_;  // not owned.
  bool has_error_ = false;
  bool panic_mode_ = false;
  Scanner* scanner_;
};

inline Parser::Parser(const string& source, Chunk* chunk) {
  scanner_ = new Scanner(source);
  chunk_ = chunk;
  curr_ = Token{TOKEN_NONE, -1, -1, -1};
  prev_ = Token{TOKEN_NONE, -1, -1, -1};
}

inline Parser::~Parser() {
  if (scanner_) delete scanner_;
}

}  // namespace qian

namespace register_prec_rule {

struct PrecRuleDefWrapper {
  PrecRuleDefWrapper(::qian::TokenType type) {
    rule = new ::qian::PrecRule();
    rule->token_type = type;
    LOG(INFO) << "register prec rule..."  << ToString(type);
    ::qian::GlobalPrecRule()->Write(rule);
  }
  PrecRuleDefWrapper& Prefix_Rule(::qian::ParseFunc f) {
    LOG(INFO) << " | prefix..."  << ToString(rule->token_type);
    rule->prefix_rule = f;
    return *this;
  }
  PrecRuleDefWrapper& Infix_Rule(::qian::ParseFunc f) {
    LOG(INFO) << " | infix..."  << ToString(rule->token_type);
    rule->infix_rule = f;
    return *this;
  }
  PrecRuleDefWrapper& Prec_Order(::qian::PrecOrder order) {
    LOG(INFO) << " | order..."  << ToString(rule->token_type);
    rule->prec_order = order;
    return *this;
  }
 private:
  ::qian::PrecRule* rule;
};

}  // namespace regsiter_prec_rule

#define REGISTER_PREC_RULE(name) REGISTER_PREC_RULE_HELPER(__COUNTER__, name)
#define REGISTER_PREC_RULE_HELPER(ctr, name) REGISTER_PREC_RULE_UNIQ(ctr, name)
#define REGISTER_PREC_RULE_UNIQ(ctr, name)                                 \
  static ::register_prec_rule::PrecRuleDefWrapper register_inst##ctr       \
    QI_ATTRIBUTE_UNUSED = ::register_prec_rule::PrecRuleDefWrapper(name)

#endif  // QIAN_COMPILER_H_
