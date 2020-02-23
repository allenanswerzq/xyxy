#include "qian/src/type.h"

#include "gtest/gtest.h"
#include "qian/src/object.h"

namespace qian {

TEST(QBoolean, TypeTest) {
  QBoolean b;
  EXPECT_EQ(b.Type(), QVAL_BOOL);
  EXPECT_EQ(b.AsCxx(), false);
  EXPECT_TRUE(IsQBoolean(b));
  EXPECT_TRUE(IsFalsey(b));

  QBoolean b2(false);
  EXPECT_EQ(b2.Type(), QVAL_BOOL);
  EXPECT_EQ(b2.AsCxx(), false);
  EXPECT_TRUE(IsQBoolean(b2));
  EXPECT_TRUE(IsFalsey(b2));
  EXPECT_TRUE(b == b2);

  EXPECT_TRUE(IsQValueEqual(b, b2));

  QBoolean b3(true);
  EXPECT_EQ(b3.Type(), QVAL_BOOL);
  EXPECT_EQ(b3.AsCxx(), true);
  EXPECT_TRUE(IsQBoolean(b3));
  EXPECT_TRUE(!IsFalsey(b3));
  EXPECT_TRUE(b != b3);
};

TEST(QFloat, TypeTest) {
  QFloat f;
  EXPECT_EQ(f.Type(), QVAL_FLOAT);
  EXPECT_EQ(f.AsCxx(), false);
  EXPECT_TRUE(IsQFloat(f));
  EXPECT_TRUE(!IsFalsey(f));

  QFloat f2;
  EXPECT_EQ(f2.Type(), QVAL_FLOAT);
  EXPECT_EQ(f2.AsCxx(), 0);
  EXPECT_TRUE(IsQFloat(f2));
  // Even the value is 0, but it's not False.
  EXPECT_TRUE(!IsFalsey(f2));
  EXPECT_TRUE(f2 == f);

  QFloat f3(1.23);
  EXPECT_EQ(f3.Type(), QVAL_FLOAT);
  EXPECT_EQ(f3.AsCxx(), 1.23);
}

TEST(QNil, TypeTest) {
  QNil nil;
  EXPECT_EQ(nil.Type(), QVAL_NIL);
  EXPECT_EQ(nil.AsCxx(), false);
  EXPECT_TRUE(IsQNil(nil));
  EXPECT_TRUE(IsFalsey(nil));
}

TEST(Misc, TypeTest) {
  QNil nil;
  QFloat f;
  EXPECT_FALSE(nil == f);
}

TEST(QString, TypeTest) {
  QString s("hello world!");
  EXPECT_TRUE(IsQString(s));
  EXPECT_TRUE(s.AsCxx());
}

}  // namespace qian
