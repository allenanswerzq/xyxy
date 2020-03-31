#ifndef QIAN_INST_H_
#define QIAN_INST_H_

#include <vector>

#include "qian/chunk.h"
#include "qian/logging.h"
#include "qian/status.h"
#include "qian/vm.h"

namespace qian {

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_NEGATE,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_NOT,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
} OpCode;

class Inst {
 public:
  virtual ~Inst() {}

  typedef std::function<void(Inst*)> FuncDebugInfo;

  typedef std::function<Status(VM*)> FuncRun;

  string Name() { return name_; }

  uint8 Length() { return length_; }

  void DebugInfo() { debug_info_(this); }

  Status Run(VM* vm) { return run_func_(vm); }

  uint8 Opcode() { return opcode_; }

  void AddOperand(Value val) { operands_.Write(val); }
  const std::vector<Value>& Operands() { return operands_; }

 protected:
  // Do not want to be called unless from its subclass.
  Inst() {}

  string name_;
  uint8 opcode_;
  uint8 length_;
  FuncRun run_func_;
  FuncDebugInfo debug_info_;
  std::vector<Value> operands_;
};

// Give a chunk and a offset index, return the Inst the bytecode representing.
std::unique_ptr<Inst> DispathInst(std::unique_ptr<Chunk> chunk, uint8 offset);

}  // namespace qian

#endif  // QIAN_INST_H_
