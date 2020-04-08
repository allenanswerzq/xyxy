#ifndef XYXY_VM_H_
#define XYXY_VM_H_

#include <memory>

#include "chunk.h"
#include "debug.h"
#include "stack.h"
#include "status.h"

namespace xyxy {

// VM class.
class VM {
 public:
  VM();
  VM(Chunk* chunk);

  ~VM() {
    if (stk_) delete stk_;
  }

  Status Run();

  Chunk* GetChunk() { return chunk_; }
  Stack<Value, STACK_SIZE>* GetStack() { return stk_; }

  uint32 PC() { return pc_; }

 private:
  Chunk* chunk_;  // not owned.
  Stack<Value, STACK_SIZE>* stk_;
  uint32 pc_;
};

inline VM::VM(Chunk* chunk) : chunk_(chunk) {
  stk_ = new Stack<Value, STACK_SIZE>();
  pc_ = 0;
}

inline VM::VM() {
  chunk_ = new Chunk();
  stk_ = new Stack<Value, STACK_SIZE>();
  pc_ = 0;
}

}  // namespace xyxy

#endif  // XYXY_VM_H_
