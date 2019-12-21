#ifndef QIAN_VM_H_
#define QIAN_VM_H_

#include "chunk.h"
#include "status.h"
#include "stack.h"
#include "debug.h"

#define MAX_STACK 256

namespace qian {

// VM class.
class VM {
 public:
  VM(Chunk* chunk);
  ~VM() { if (stk_) delete stk_; }

  Status Run();
  Status Run(Inst* inst);

  Chunk* GetChunk() { return chunk_; }
  Stack<Value, MAX_STACK>* GetStack() { return stk_; }

 private:
  Chunk* chunk_; // not owned.
  Stack<Value, MAX_STACK>* stk_;
  uint32_t pc_;
  Disambler* disambler_;
};

inline VM::VM(Chunk* chunk) : chunk_(chunk) {
  stk_ = new Stack<Value, MAX_STACK> ();
  disambler_ = new Disambler(chunk, "default_vm");
  pc_ = 0;
}

typedef std::function<Status(VM*)> InstRunFunc;

static Vector<InstRunFunc>* GlobalFunc() {
  static Vector<InstRunFunc>* registry_;
  if (registry_ == nullptr) {
    registry_ = new Vector<InstRunFunc>;
  }
  return registry_;
}
}  // namespace qian

namespace register_func {

using ::qian::InstRunFunc;

struct InstRunDefWrapper {
  InstRunDefWrapper() {}
  InstRunDefWrapper& Func(const InstRunFunc& f) {
    ::qian::GlobalFunc()->Write(f);
    return *this;
  }
  InstRunDefWrapper& Name(const string& name) {
    (void) name;
    return *this;
  }
};

} // namespace register_func

#define REGISTER_FUNC() REGISTER_FUNC_UNIQ_HELPER(__COUNTER__)
#define REGISTER_FUNC_UNIQ_HELPER(ctr) REGISTER_FUNC_UNIQ(ctr)
#define REGISTER_FUNC_UNIQ(ctr)                                 \
  static ::register_func::InstRunDefWrapper register_func##ctr     \
    __attribute__ ((unused)) = ::register_func::InstRunDefWrapper()

namespace qian {

REGISTER_FUNC()
  .Name("OP_RETURN")
  .Func([](VM* vm) {
    return Status();
  });

REGISTER_FUNC()
  .Name("OP_CONSTANT")
  .Func([](VM* vm) {
    return Status();
  });
}  // namespace qian

#endif  // QIAN_VM_H_
