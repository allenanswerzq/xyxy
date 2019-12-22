#include "gtest/gtest.h"
#include "vm.h"

namespace qian {
  Chunk chunk;
  chunk.WriteByte(0);
  chunk.WriteChunk(1, 0, 1.23, 0);

  VM vm(&chunk);
  vm.Run();
}  // namespace qian
