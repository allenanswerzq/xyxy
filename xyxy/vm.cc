#include "vm.h"

#include "inst_impl.h"
#include "type.h"

namespace xyxy {

VM::VM(std::shared_ptr<Chunk> chunk) : chunk_(chunk) {
  stk_ = std::make_shared<Stack<Value, STACK_SIZE>>();
  pc_ = 0;
}

VM::VM() {
  chunk_ = std::make_shared<Chunk>();
  stk_ = std::make_shared<Stack<Value, STACK_SIZE>>();
  pc_ = 0;
}

Status VM::Run() {
  for (; pc_ < chunk_->Size();) {
    auto inst = DispathInst(chunk_, pc_);
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

}  // namespace xyxy
