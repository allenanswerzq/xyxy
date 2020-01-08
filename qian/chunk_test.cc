#include "chunk.h"

#include "gtest/gtest.h"
#include "type.h"

namespace qian {

TEST(WriteSmall, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 7; i++) {
    chunk.WriteByte(i);
  }
  for (int i = 0; i < 7; i++) {
    EXPECT_EQ(chunk.GetByte(i), i);
  }
}

TEST(WriteBig, TestChunk) {
  Chunk chunk;
  const int n = 1000000;
  for (int i = 0; i < n; i++) {
    chunk.WriteByte(i & 0xff);
  }
  for (int i = 0; i < n; i++) {
    EXPECT_EQ(chunk.GetByte(i), i & 0xff);
  }
}

TEST(WriteChunk, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 4; i++) {
    chunk.WriteChunk(0, 0);
  }
  for (int i = 0; i < 8; i++) {
    chunk.WriteChunk(i, 1.23 + i, QIAN_NUMBER(i));
  }
}

TEST(AddValue, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 4; i++) {
    chunk.AddValue(QIAN_NUMBER(i));
  }
  for (int i = 0; i < 4; i++) {
    EXPECT_FLOAT_EQ(i, AS_CXX_NUMBER(chunk.GetValue(i)));
  }
}

}  // namespace qian
