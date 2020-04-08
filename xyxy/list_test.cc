#include "xyxy/list.h"

#include "gtest/gtest.h"

namespace xyxy {

struct DummyClass {
  DummyClass() { count_++; }

  ~DummyClass() { count_--; }

  static int count_;
};

int DummyClass::count_ = 0;

TEST(Pointers, TestList) {
  {
    List<DummyClass*> v;
    EXPECT_EQ(DummyClass::count_, 0);
    v.AppendTail(new DummyClass());
    v.AppendTail(new DummyClass());
    v.AppendTail(new DummyClass());
    EXPECT_EQ(DummyClass::count_, 3);
  }
  EXPECT_EQ(DummyClass::count_, 0);
}

TEST(Next, TestList) {
  List<int> v;
  v.AppendTail(1);
  v.AppendTail(2);
  v.AppendTail(4);
}

}  // namespace xyxy
