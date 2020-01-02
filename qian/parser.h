#ifndef QIAN_PARSE_H_
#define QIAN_PARSE_H_

#include <functional>

#include "type.h"
#include "chunk.h"
#include "vector.h"
#include "scanner.h"

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
  Parser() {}
  ~Parser();

  Parser(const string& source, Chunk* chunk);

  void advance();
  void consume(TokenType type, const string& msg);

  void number();
  int make_constant(Value val);
  void emit_constant(Value val);

  void emit_byte(uint8 byte);
  void emit_byte(uint8 byte1, uint8 byte2);
  void emit_return();

  void parse_with_prec_order(PrecOrder prec_order);

  void grouping();
  void unary();
  void binary();
  void expression();
  void literal();

  Chunk* GetChunk() { return chunk_; }

  Token prev_token() { return prev_; }
  Token curr_token() { return curr_; }

  string get_lexeme(Token tk) {
    return scanner_->get_lexeme(tk);
  }

 private:
  PrecRule* get_rule(TokenType type) {
    return GlobalPrecRule()->Get(type);
  }

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
  curr_ = CreateToken(TOKEN_NONE, -1, -1, -1);
  prev_ = CreateToken(TOKEN_NONE, -1, -1, -1);
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
    ::qian::GlobalPrecRule()->Write(rule);
  }
  PrecRuleDefWrapper& Prefix_Rule(::qian::ParseFunc f) {
    rule->prefix_rule = f;
    return *this;
  }
  PrecRuleDefWrapper& Infix_Rule(::qian::ParseFunc f) {
    rule->infix_rule = f;
    return *this;
  }
  PrecRuleDefWrapper& Prec_Order(::qian::PrecOrder order) {
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
