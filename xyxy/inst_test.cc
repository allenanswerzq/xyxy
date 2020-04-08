#include "xyxy/inst.h"

#include "gtest/gtest.h"
#include "xyxy/chunk.h"
#include "xyxy/inst_impl.h"

namespace xyxy {

TEST(Basic, TestInst) {
  auto chunk = std::make_shared<Chunk>();
  chunk->Write(OP_RETURN);
  auto inst = DispathInst(chunk, 0);
  EXPECT_EQ(inst->Name(), "OP_RETURN");
  EXPECT_EQ(inst->Length(), 1);
  // EXPECT_TRUE(inst->DebugInfo() != nullptr);
  EXPECT_EQ(inst->Opcode(), OP_RETURN);
}

}  // namespace xyxy
