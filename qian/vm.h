#ifndef QIAN_VM_H_
#define QIAN_VM_H_

#include <memory>

#include "chunk.h"
#include "debug.h"
#include "inst.h"
#include "stack.h"
#include "status.h"

namespace qian {

// VM class.
class VM {
 public:
  VM();
  VM(Chunk* chunk);

  ~VM() {
    if (stk_) delete stk_;
  }

  Status Run();
  Status Run(std::unique_ptr<Inst>& inst);

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

struct InstRunDefWrapper {
  // TDDO(zq7): add santiy check to ensure the inst defined before adding func.
  InstRunDefWrapper() {}
  InstRunDefWrapper& Func(const ::qian::InstRunFunc& f) {
    ::qian::GlobalFunc()->Write(f);
    return *this;
  }
  InstRunDefWrapper& Name(const string& name) {
    (void)name;
    return *this;
  }
};

}  // namespace register_func

#define REGISTER_FUNC() REGISTER_FUNC_UNIQ_HELPER(__COUNTER__)
#define REGISTER_FUNC_UNIQ_HELPER(ctr) REGISTER_FUNC_UNIQ(ctr)
#define REGISTER_FUNC_UNIQ(ctr)                                \
  static ::register_func::InstRunDefWrapper register_func##ctr \
      QI_ATTRIBUTE_UNUSED = ::register_func::InstRunDefWrapper()

#endif  // QIAN_VM_H_
