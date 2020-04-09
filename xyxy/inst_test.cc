#include "xyxy/inst.h"

#include "gtest/gtest.h"
#include "xyxy/chunk.h"
#include "xyxy/inst_impl.h"

namespace xyxy {

TEST(OP_RETURN, TestInst) {
  auto chunk = std::make_shared<Chunk>();
  chunk->Write(OP_RETURN);
  auto inst = DispathInst(chunk, 0);
  EXPECT_EQ(inst->Name(), "OP_RETURN");
  EXPECT_EQ(inst->Length(), 1);
  EXPECT_EQ(inst->Opcode(), OP_RETURN);
}

TEST(OP_CONSTANT, TestInst) {
  auto chunk = std::make_shared<Chunk>();
  chunk->Write(OP_CONSTANT);
  chunk->Write(0);
  chunk->AddConstant(XYXY_NIL);
  auto inst = DispathInst(chunk, 0);
  EXPECT_EQ(inst->Name(), "OP_CONSTANT");
  EXPECT_EQ(inst->Length(), 2);
  EXPECT_EQ(inst->Opcode(), OP_CONSTANT);
}

}  // namespace xyxy
