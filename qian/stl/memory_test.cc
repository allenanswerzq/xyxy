#include "qian/stl/memory.h"

#include "gtest/gtest.h"
#include "qian/stl/vector.h"

namespace stl {

struct Dummy {
  Dummy() { count_++; }

  ~Dummy() { count_--; }

  static int count_;
};

int Dummy::count_ = 0;

TEST(Simple, TestUniquePtr) {
  EXPECT_EQ(Dummy::count_, 0);
  {
    auto a = MakeUnique<Dummy>();
    EXPECT_EQ(Dummy::count_, 1);
  }
  EXPECT_EQ(Dummy::count_, 0);
}

TEST(Vector, TestUniquePtr) {
  {
    Vector<Unique_Ptr<Dummy>> v(10);
    for (int i = 0; i < 10; i++) {
      EXPECT_EQ(Dummy::count_, i);
      v[i] = MakeUnique<Dummy>();
    }
  }
  EXPECT_EQ(Dummy::count_, 0);
}

}  // namespace stl
