#include "vm.h"

#include "chunk.h"
#include "gtest/gtest.h"

namespace qian {

TEST(Run, TestVM) {
  Chunk chunk;
  chunk.WriteChunk(OP_CONSTANT, 0, QIAN_NUMBER(1.23), 0);
  chunk.WriteByte(OP_RETURN);
  EXPECT_EQ(chunk.GetByte(0), int(OP_CONSTANT));
  EXPECT_EQ(chunk.GetByte(1), int(OP_RETURN));
  EXPECT_EQ(chunk.GetValue(0), QIAN_NUMBER(1.23));
  VM vm(&chunk);
  vm.Run();
}

}  // namespace qian
