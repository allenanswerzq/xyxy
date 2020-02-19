#include "stl/list.h"

#include "gtest/gtest.h"
#include "stl/vector.h"

namespace stl {

TEST(Next, TestList) {
  List<int> v;
  v.AppendTail(1);
  v.AppendTail(2);
  v.AppendTail(3);
  auto node = v.GetHead();
  for (int i = 0; i < 3; i++) {
    EXPECT_EQ(node->value, i + 1);
    node = node->next;
  }
}

TEST(Vector, TestList) {
  Vector<List<int>> v(2);
  for (int i = 0; i < 3; i++) {
    v[0].AppendTail(i);
  }
  for (int i = 0; i < 3; i++) {
    v[1].AppendTail(i + 3);
  }
}

}  // namespace stl
