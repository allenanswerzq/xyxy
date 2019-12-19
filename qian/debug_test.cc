#include "gtest/gtest.h"

#include "chunk.h"
#include "inst.h"
#include "debug.h"

namespace qian {

TEST(TestReturnInst, TestDisassembler) {
  Chunk chunk;
  chunk.WriteChunk(0);

  ::testing::internal::CaptureStdout();
  DisambleChunk(&chunk, "test_op");
  std::string str = ::testing::internal::GetCapturedStdout();
  EXPECT_EQ(str, "== test_op ==\n0000 OP_RETURN\n");
}

} // namespace qian
