#include "xyxy/stack.h"

#include "gtest/gtest.h"
#include "xyxy/type.h"

namespace xyxy {

TEST(Push, TestStack) {
  const int N = 256;
  Stack<int, N> stk;
  EXPECT_TRUE(stk.Empty());
  EXPECT_FALSE(stk.Full());
  for (int i = 0; i < N; i++) {
    stk.Push(i);
  }
  for (int i = 0; i < N; i++) {
    EXPECT_EQ(stk.Pop(), N - 1 - i);
  }
}

TEST(Top, TestStack) {
  const int N = 256;
  Stack<Value, N> stk;
  stk.Push(Value(1));
  stk.Push(Value(2));
  auto x = stk.Top();
  auto y = stk.Pop();
  // EXPECT_EQ(x.ToString(), "2");
  // EXPECT_EQ(y.ToString(), "2");
  EXPECT_TRUE(x == y);
}

TEST(PushDeath, TestStack) {
  const int N = 16;
  Stack<int, N> stk;
  EXPECT_TRUE(stk.Empty());
  EXPECT_FALSE(stk.Full());
  for (int i = 0; i < 20; i++) {
    if (i >= N) {
      EXPECT_DEATH({ stk.Push(i); }, "failed");
    }
    else {
      stk.Push(i);
    }
  }
}

}  // namespace xyxy
