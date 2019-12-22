#include "gtest/gtest.h"
#include "chunk.h"
#include "vm.h"

namespace qian {

TEST(Run, TestVM) {
  Chunk chunk;
  chunk.WriteChunk(OP_CONSTANT, 0, 1.23, 0);
  chunk.WriteByte(OP_RETURN);

  VM vm(&chunk);
  vm.Run();
}

}  // namespace qian
