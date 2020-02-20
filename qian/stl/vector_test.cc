#include "qian/stl/vector.h"

#include "gtest/gtest.h"
#include "qian/stl/memory.h"

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

TEST(Operator, TestVector) {
  Vector<int> v(1024);
  EXPECT_EQ(v.Size(), 1024);
  for (int i = 0; i < 1024; i++) {
    v[i] = i;
  }
  for (int i = 0; i < 1024; i++) {
    EXPECT_EQ(v[i], i);
  }
}

TEST(Reverse, TestVector) {
  Vector<int> v;
  v.Reserve(1024);
  EXPECT_EQ(v.Size(), 1024);
  for (int i = 0; i < 1024; i++) {
    v[i] = i;
  }
  for (int i = 0; i < 1024; i++) {
    EXPECT_EQ(v[i], i);
  }
}

struct DummyClass {
  DummyClass() { count_++; }

  ~DummyClass() { count_--; }

  static int count_;
};

int DummyClass::count_ = 0;

TEST(Pointers, TestVector) {
  {
    Vector<Unique_Ptr<DummyClass>> v;
    EXPECT_EQ(DummyClass::count_, 0);
    v.Write(WrapUnique(new DummyClass()));
    v.Write(WrapUnique(new DummyClass()));
    v.Write(WrapUnique(new DummyClass()));
    EXPECT_EQ(DummyClass::count_, 3);
  }
  EXPECT_EQ(DummyClass::count_, 0);
}

}  // namespace stl
