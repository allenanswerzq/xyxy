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

}

}  // namespace qian
