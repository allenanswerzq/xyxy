#ifndef XYXY_VM_H_
#define XYXY_VM_H_

#include <memory>

#include "xyxy/chunk.h"
#include "xyxy/hash_table.h"
#include "xyxy/object.h"
#include "xyxy/stack.h"
#include "xyxy/status.h"
#include "xyxy/type.h"

#define FRAME_MAX 64
#define STACK_SIZE (256 * FRAME_MAX)

namespace xyxy {

enum OpCode {
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
  OP_LOOP,
  OP_CALL,
};

// Forward declaration.
class VM;

struct CallFrame {
  // The function that is current running.
  ObjFunction* function;
  int ip;
  int offset;
};

class VM {
 public:
  explicit VM(ObjFunction* function);

  virtual ~VM() = default;

  Status Run();

  Chunk* GetChunk() { return curr_frame_.function->GetChunk(); }

  void DumpStack();

  void DisassFrame(CallFrame frame);

  std::string FinalResult() { return final_print_; }

  Stack<Value, STACK_SIZE>& GetStack() { return stack_; }

  hash_table<string, Value>& GetGlobal() { return global_; }

  uint8 ReadByte() {
    CHECK(curr_frame_.ip < GetChunk()->size());
    return GetChunk()->GetByte(curr_frame_.ip++);
  }

  uint16 ReadShort() {
    uint8 hi = ReadByte();
    uint8 lo = ReadByte();
    curr_frame_.ip += 2;
    return (hi << 8) | lo;
  }

  Value ReadConstant() {
    int index = ReadByte();
    return GetChunk()->GetConstant(index);
  }

  Status Call(Value* callee, int arg_count);
  Status Call(ObjFunction* callee, int arg_count);

  Value GetFrame(int idx) { return stack_.Get(curr_frame_.offset + idx); }

  void SetFrame(int idx, Value val) {
    stack_.Set(curr_frame_.offset + idx, val);
  }

 private:
  const int kDisassWidth = 30;
  // A simple way to remember the last print result for verifying,
  // TODO(): not only verfiy the final result, but also the intermediate
  // execution result.
  std::string final_print_;
  // Chunk* chunk_;  // Not owned.
  // The virtual machine stack.
  Stack<Value, STACK_SIZE> stack_;
  // Current running frame.
  CallFrame curr_frame_;
  // Call frames.
  std::vector<CallFrame> frames_;
  // Store all global variabls.
  hash_table<string, Value> global_;
};

}  // namespace xyxy

#endif  // XYXY_VM_H_
