#include "gtest/gtest.h"
#include "debug.h"

namespace qian {

TEST(Return, TestDisassembler) {
  Chunk chunk;
  chunk.WriteByte(OP_RETURN);

  ::testing::internal::CaptureStdout();
  DisambleChunk(&chunk, "test_op");
  std::string str = ::testing::internal::GetCapturedStdout();
  EXPECT_EQ(str, "== test_op ==\n0000 OP_RETURN\n");
}

} // namespace qian
