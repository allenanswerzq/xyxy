#include "xyxy/type.h"

#include <memory>

#include "gtest/gtest.h"

namespace xyxy {

TEST(Boolean, TypeTest) {
  Value b(false);
  EXPECT_EQ(b.Type(), ValueType::VAL_BOOL);
  EXPECT_EQ(b.AsBool(), false);
  EXPECT_TRUE(b.IsBool());
  EXPECT_TRUE(b.IsFalsey());

  Value b2(false);
  EXPECT_EQ(b2.Type(), ValueType::VAL_BOOL);
  EXPECT_EQ(b2.AsBool(), false);
  EXPECT_TRUE(b2.IsBool());
  EXPECT_TRUE(b2.IsFalsey());
  EXPECT_TRUE(b == b2);

  Value b3(true);
  EXPECT_EQ(b3.Type(), ValueType::VAL_BOOL);
  EXPECT_EQ(b3.AsBool(), true);
  EXPECT_TRUE(b3.IsBool());
  EXPECT_TRUE(!b3.IsFalsey());
  EXPECT_TRUE(b != b3);
};

TEST(Float, TypeTest) {
  Value f(0.0);
  EXPECT_EQ(f.Type(), ValueType::VAL_FLOAT);
  EXPECT_EQ(f.AsFloat(), false);
  EXPECT_TRUE(f.IsFloat());
  EXPECT_TRUE(!f.IsFalsey());

  Value f2(0.0);
  EXPECT_EQ(f2.Type(), ValueType::VAL_FLOAT);
  EXPECT_EQ(f2.AsFloat(), 0);
  EXPECT_TRUE(f.IsFloat());
  EXPECT_TRUE(!f.IsFalsey());
  EXPECT_TRUE(f2 == f);

  Value f3(1.23);
  EXPECT_EQ(f3.Type(), ValueType::VAL_FLOAT);
  EXPECT_EQ(f3.AsFloat(), 1.23);
}

TEST(Nil, TypeTest) {
  Value nil;
  EXPECT_EQ(nil.Type(), ValueType::VAL_NIL);
  EXPECT_EQ(nil.AsNil(), false);
  EXPECT_TRUE(nil.IsNil());
  EXPECT_TRUE(nil.IsFalsey());
}

TEST(String, TypeTest) {
  std::unique_ptr<ObjString> p(new ObjString("hello world"));
  Value val(p.get());
  EXPECT_TRUE(val.IsString());
  EXPECT_EQ(val.AsString(), "hello world");
  EXPECT_EQ(val.ToString(), "hello world");
}

}  // namespace xyxy
