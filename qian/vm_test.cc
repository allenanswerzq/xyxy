#include "vm.h"

#include "chunk.h"
#include "gtest/gtest.h"

namespace qian {

TEST(Run, TestVM) {
  Chunk chunk;
  chunk.WriteChunk(OP_CONSTANT, 0, QIAN_NUMBER(1.23), 0);
  chunk.WriteByte(OP_RETURN);

  VM vm(&chunk);
  vm.Run();
}

}  // namespace qian
