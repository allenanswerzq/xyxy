#ifndef XYXY_VM_H_
#define XYXY_VM_H_

#include <memory>

#include "xyxy/chunk.h"
#include "xyxy/stack.h"
#include "xyxy/status.h"

namespace xyxy {

// VM class.
class VM {
 public:
  VM();
  VM(std::shared_ptr<Chunk> chunk);

  ~VM() = default;

  Status Run();

  std::shared_ptr<Chunk> GetChunk() { return chunk_; }
  std::shared_ptr<Stack<Value, STACK_SIZE>> GetStack() { return stk_; }

  uint32 PC() { return pc_; }

 private:
  std::shared_ptr<Chunk> chunk_;
  std::shared_ptr<Stack<Value, STACK_SIZE>> stk_;
  uint32 pc_;
};

}  // namespace xyxy

#endif  // XYXY_VM_H_
