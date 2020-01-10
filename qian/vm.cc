#include "vm.h"

#include "inst_impl.h"
#include "type.h"

namespace qian {

Status VM::Run() {
  for (; pc_ < chunk_->Size();) {
    std::unique_ptr<Inst> inst(DispathInst(chunk_, pc_));
#ifndef NDEBUG
    inst->DebugInfo();
#endif
    Status status = inst->Run(this);
    pc_ += inst->Length();
    // TODO(zq7): Handle errors according to different status.
    // Add something like this:
    // error_->HandleError(status);
  }
  return Status();
}

}  // namespace qian
