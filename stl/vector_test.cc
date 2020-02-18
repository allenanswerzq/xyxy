#include "stl/vector.h"

#include <vector>

#include "gtest/gtest.h"

namespace stl {

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
  const int *addr = v.Address();
  for (int i = 0; i < 1024; i++) {
    EXPECT_EQ(addr[i], i);
  }
}

struct DummyClass {
  DummyClass() { count_++; }

  ~DummyClass() { count_--; }

  static int count_;
};

int DummyClass::count_ = 0;

// TEST(Pointers, TestVector) {
//   {
//     Vector<std::unique_ptr<DummyClass>> v;
//     EXPECT_EQ(DummyClass::count_, 0);
//     v.Write(WrapUnique(new DummyClass()));
//     v.Write(WrapUnique(new DummyClass()));
//     v.Write(WrapUnique(new DummyClass()));
//     EXPECT_EQ(DummyClass::count_, 3);
//   }
//   EXPECT_EQ(DummyClass::count_, 0);
// }

TEST(StdVector, TestVector) {
  {
    std::vector<std::unique_ptr<DummyClass>> v;
    EXPECT_EQ(DummyClass::count_, 0);
    v.push_back(WrapUnique(new DummyClass()));
    v.push_back(WrapUnique(new DummyClass()));
    v.push_back(WrapUnique(new DummyClass()));
    EXPECT_EQ(DummyClass::count_, 3);
  }
  EXPECT_EQ(DummyClass::count_, 0);
}

}  // namespace stl
