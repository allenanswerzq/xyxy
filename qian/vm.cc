#include "vm.h"

namespace qian {

Status VM::Run() {
  for (; pc_ < chunk_->Size(); ) {
    Inst* inst = DispathInst(chunk_, pc_);
#ifndef NDEBUG
    disambler_->DisambleInst(inst);
#endif
    Run(inst);
    pc_ += inst->Length();
  }
  return Status();
}

Status VM::Run(Inst* inst) {
  auto f = GlobalFunc()->Get(inst->Opcode());
  return f(this);
}

REGISTER_FUNC()
  .Name("OP_RETURN")
  .Func([](VM* vm) {
    vm->GetStack()->Dump();
    return Status();
  });

REGISTER_FUNC()
  .Name("OP_CONSTANT")
  .Func([](VM* vm) {
    // Push something onto the stack.
    vm->GetStack()->Push(1.24);
    return Status();
  });

}  // namespace qian
