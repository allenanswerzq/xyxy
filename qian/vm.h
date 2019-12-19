#ifndef QIAN_VM_H_
#define QIAN_VM_H_

#include "status.h"
#include "chunk.h"

namespace qian {
class VM {
 public:
  VM(Chunk const *chunk) : chunk_(chunk) {}

  Status interpret();

 private:
  Chunk const *chunk_; // not owned.
};

} // namespace qian


#endif  // QIAN_VM_H_
