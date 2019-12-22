#include "vm.h"

namespace qian {

Status VM::Run() {
  for (;;) {
    Inst* inst = DispathInst(chunk_, pc_);
#ifndef NDEBUG
    disambler_->DisambleInst(inst);
#endif
    Run(inst);
    pc_ += inst->Length();
  }
}

Status VM::Run(Inst* inst) {
  auto f = GlobalFunc()->Get(inst->Opcode());
  return f(this);
}

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
