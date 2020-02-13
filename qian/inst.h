#ifndef QIAN_INST_H_
#define QIAN_INST_H_

#include "chunk.h"
#include "logging.h"
#include "status.h"
#include "vm.h"

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

  void Operand(Value val) { operands_.Write(val); }
  Vector<Value>* Operands() { return &operands_; }

 protected:
  Inst() {}

  string name_;
  uint8 opcode_;
  uint8 length_;
  FuncRun run_func_;
  FuncDebugInfo debug_info_;
  Vector<Value> operands_;
};

Inst* CreateInst(OpCode byte);

Inst* DispathInst(Chunk* chunk, uint8 offset);

}  // namespace qian

#endif  // QIAN_INST_H_
