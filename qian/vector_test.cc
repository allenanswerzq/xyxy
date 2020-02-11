#include "vector.h"

#include "gtest/gtest.h"

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
  const int *addr = v.Address();
  for (int i = 0; i < 1024; i++) {
    EXPECT_EQ(addr[i], i);
  }
}

struct DummyClass {
  DummyClass() {
    count_++;
  }

  ~DummyClass() {
    count_--;
  }

  static int count_;
};

int DummyClass::count_ = 0;

TEST(Pointers, TestVector) {
  {
    Vector<DummyClass*> v;
    EXPECT_EQ(DummyClass::count_, 0);
    v.Write(new DummyClass());
    v.Write(new DummyClass());
    v.Write(new DummyClass());
    EXPECT_EQ(DummyClass::count_, 3);
  }
  EXPECT_EQ(DummyClass::count_, 0);
}

}  // namespace qian
