#ifndef XYXY_VM_H_
#define XYXY_VM_H_

#include <memory>

#include "xyxy/chunk.h"
#include "xyxy/hash_table.h"
#include "xyxy/stack.h"
#include "xyxy/status.h"

namespace xyxy {

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
  OP_PRINT,
  OP_POP,
  OP_DEFINE_GLOBAL,
  OP_SET_GLOBAL,
  OP_GET_GLOBAL,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_JUMP_IF_FALSE,
  OP_JUMP,
} OpCode;

// Forward declaration.
class VM;

class Inst {
 public:
  Inst() {}
  virtual ~Inst() {}

  string Name() { return name_; }

  uint8 Length() { return length_; }

  void DebugInfo();

  uint8 Opcode() { return opcode_; }

  void AddOperand(Value val) { operands_.push_back(val); }

  std::vector<Value>& Operands() { return operands_; }

 protected:
  friend class VM;
  static const int kDumpWidth;
  string name_;
  uint8 opcode_;
  uint8 length_;
  std::vector<Value> operands_;
  // Stores some meta data to use.
  // TODO(): may make this into a class.
  std::vector<uint8> metadata_;
};

class VM {
 public:
  explicit VM(Chunk* chunk);

  virtual ~VM() = default;

  Status Run();

  std::unique_ptr<Inst> CreateInst(uint8 offset);

  Chunk* GetChunk() { return chunk_; }

  void DumpInsts();

  std::string FinalResult() { return final_print_; }

  Stack<Value, STACK_SIZE>& GetStack() { return stack_; }

  hash_table<string, Value>& GetGlobal() { return global_; }

  uint32 PC() { return pc_; }

 private:
  // A simple way to remember the last print result for verifying,
  // TODO(): not only verfiy the final result, but also the intermediate
  // execution result.
  std::string final_print_;
  Chunk* chunk_;  // Not owned.
  // Virtual machine stack.
  Stack<Value, STACK_SIZE> stack_;
  // Store all global variabls.
  hash_table<string, Value> global_;
  uint32 pc_;
};

}  // namespace xyxy

#endif  // XYXY_VM_H_
