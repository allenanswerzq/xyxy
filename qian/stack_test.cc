#include "gtest/gtest.h"
#include "stack.h"

namespace qian {

TEST(Push, TestStack) {
  const int N = 256;
  Stack<int, N> stk;
  EXPECT_TRUE(stk.Empty());;
  EXPECT_FALSE(stk.Full());;
  for (int i = 0; i < N; i++) {
    stk.Push(i);
  }
  for (int i = 0; i < N; i++) {
    EXPECT_EQ(stk.Pop(), N - 1 - i);
  }
}

TEST(PushDeath, TestStack) {
  const int N = 256;
  Stack<int, N> stk;
  EXPECT_TRUE(stk.Empty());;
  EXPECT_FALSE(stk.Full());;
  for (int i = 0; i < 300; i++) {
    if (i >= N)  {
      EXPECT_DEATH({
        stk.Push(i);
      }, "Assertion failed");
    } else {
      stk.Push(i);
    }
  }
}

TEST(DebugInfo, TestStack) {
  Stack<int, 4> stk;
  for (int i = 0; i < 4; i++) {
    stk.Push(i);
  }
  stk.DebugInfo();
}

} // namespace qian