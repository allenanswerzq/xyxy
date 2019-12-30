#ifndef QIAN_PARSE_H_
#define QIAN_PARSE_H_

#include <functional>

#include "vector.h"
#include "scanner.h"

namespace qian {

typedef std::function<void()> ParseFunc;

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

typedef struct {
  TokenType token_type = TOKEN_NONE;
  ParseFunc prefix_rule = nullptr;
  ParseFunc infix_rule = nullptr;
  PrecOrder prec_order = PREC_NONE;
} PrecRule

class Parser {
 public:
  Parser(const string& source);
  ~Parser();

  void advance();
  void consume();

  void number();
  int make_constant(Value val);

  void emit_byte(uint8 byte);
  void emit_byte(uint8 byte1, uint8 byte2);

  parse_with_prec_order(PrecOrder prec_order);

  void grouping();

 private:
  struct Wrapper {
    Wrapper(TokenType type) {
      rule = new PrecRule();
      rule->type = type;
      registry_->Write(rule);
    }

    Wrapper& Prefix_Rule(ParseFunc f) {
      rule->prefix_rule = f;
      return *this;
    }

    Wrapper& Prefix_Rule(ParseFunc f) {
      rule->infix_rule = f;
      return *this;
    }

    Wrapper& Prec_Order(PrecOrder order) {
      rule->prec_order = order;
      return *this;
    }
    PrecRule* rule;
  };

  Wrapper& create_rule_wrapper(TokenType type) {
    return Wrapper(type);
  }

  Chunk* chunk_;  // not owned.
  Token curr_;
  Token prev_;
  bool has_error_ = false;
  bool panic_mode_ = false;
  Scanner* scanner_;
  Vector<PrecRule*>* registry_;

};

#define EGISTER_PREC_RULE(type) create_rule_wrapper(type)

Parser::Parser(const string& source) {
  scanner_ = new Scanner(source);

  REGISTER_PREC_RULE(TOKEN_LEFT_PAREN)
    .Prefix_Rule(grouping)
    .Infix_Rule(nullptr)
    .Prec_Order(PREC_NONE);

}

Parser::~Parser() {
  if (scanner_) delete scanner_;
}

}  // namespace qian

#endif  // QIAN_COMPILER_H_
