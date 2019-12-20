#ifndef QIAN_VM_H_
#define QIAN_VM_H_

#include "status.h"
#include "stack.h"
#include "debug.h"

namespace qian {

#define MAX_STACK 256

class VM {
 public:
  VM(Chunk* chunk);
  ~VM() { if (stk_) delete stk_; }

  Status interpret();
  Status run();

  Chunk* GetChunk() { return chunk_; }
  Stack<Value, MAX_STACK>* GetStack() { return stk_; }

 private:
  Chunk* chunk_; // not owned.
  Stack<Value, MAX_STACK>* stk_;
  uint32_t pc_;
  Disambler* disambler_;
};

VM::VM(Chunk* chunk) : chunk_(chunk) {
  stk_ = new Stack<Value, MAX_STACK> ();
  disambler_ = new Disambler(chunk, "default_vm");
}

Status VM::interpret() {
  pc_ = 0;
  return run();
}

Status VM::run() {
  for (;;) {
    Inst* inst = DispathInst(chunk_, pc_);
#ifndef NDEBUG
    disambler_->DisambleInst(inst);
#endif
    inst->Run(this);
    pc_ += inst->Length();
  }
}

} // namespace qian


#endif  // QIAN_VM_H_
