#include "gtest/gtest.h"

#include "chunk.h"

namespace {

TEST(TestChunkSmall, TestChunkWrite) {
  Chunk chunk;
  for (int i = 0; i < 7; i++) {
    chunk.WriteChunk(i);
  }
  for (int i = 0; i < 7; i++) {
    EXPECT_EQ(chunk.GetCode(i), i);
  }
}

TEST(TestChunkBig, TestChunkWrite) {
  Chunk chunk;
  const int n = 1000000;
  for (int i = 0; i < n; i++) {
    chunk.WriteChunk(i & 0xff);
  }
  for (int i = 0; i < n; i++) {
    EXPECT_EQ(chunk.GetCode(i), i & 0xff);
  }
}

}
