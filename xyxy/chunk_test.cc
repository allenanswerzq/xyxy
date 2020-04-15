#include "xyxy/chunk.h"

#include "gtest/gtest.h"
#include "xyxy/type.h"

namespace xyxy {

TEST(WriteSmall, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 7; i++) {
    chunk.Write(i);
  }
  for (int i = 0; i < 7; i++) {
    EXPECT_EQ(chunk.GetByte(i), i);
  }
}

TEST(WriteBig, TestChunk) {
  Chunk chunk;
  const int n = 1000000;
  for (int i = 0; i < n; i++) {
    chunk.Write(i & 0xff);
  }
  for (int i = 0; i < n; i++) {
    EXPECT_EQ(chunk.GetByte(i), i & 0xff);
  }
}

TEST(Float, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 4; i++) {
    chunk.Write(0, 0);
  }
  for (int i = 0; i < 8; i++) {
    chunk.Write(i, i, Value(i));
  }
}

TEST(Constant, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 4; i++) {
    chunk.AddConstant(Value(i));
  }
  for (int i = 0; i < 4; i++) {
    EXPECT_FLOAT_EQ(i, chunk.GetConstant(i).AsFloat());
  }
}

TEST(WriteAt, TestChunk) {
  Chunk chunk;
  for (int i = 0; i < 4; i++) {
    chunk.Write(i, 0);
  }
  EXPECT_EQ(chunk.size(), 4);
  chunk.WriteAt(2, 122);
  EXPECT_FLOAT_EQ(122, chunk.GetByte(2));
}
}  // namespace xyxy
