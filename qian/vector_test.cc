#include "gtest/gtest.h"
#include "vector.h"

namespace qian {

TEST(WriteGet, TestVector) {
  Vector<int> v;
  EXPECT_EQ(v.Size(), 0);
  for (int i = 0; i < 1024; i++) {
    v.Write(i);
  }
  for (int i = 0; i < 1024; i++) {
    EXPECT_EQ(v.Get(i), i);
  }
}

TEST(Address, TestVector) {
  Vector<int> v;
  EXPECT_EQ(v.Size(), 0);
  for (int i = 0; i < 1024; i++) {
    v.Write(i);
  }
  int const *addr = v.Address();
  for (int i = 0; i < 1024; i++) {
    EXPECT_EQ(addr[i], i);
  }
}

} // namespace qian
