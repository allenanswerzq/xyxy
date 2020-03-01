#include "qian/stl/hash_table.h"

#include <string>

#include "gtest/gtest.h"

namespace qian {

TEST(String, hash_tableTest) {
  hash_table<std::string, int> ht;
  ht.Insert("foo", 1);
  ht.Insert("bar", 2);
  EXPECT_TRUE(ht.Find("foo"));
  EXPECT_TRUE(ht.Find("bar"));
  int val;
  ht.Find("foo", &val);
  EXPECT_EQ(val, 1);
  ht.Find("bar", &val);
  EXPECT_EQ(val, 2);
  EXPECT_FALSE(ht.Find("tar"));
}

TEST(String, hash_setTest) {
  hash_set<std::string> hs;
  hs.Insert("foo");
  hs.Insert("bar");
  EXPECT_TRUE(hs.Find("foo"));
  EXPECT_TRUE(hs.Find("bar"));
  EXPECT_FALSE(hs.Find("tar"));
}

TEST(NoDefineHash, hash_setTest) {
  struct Simple {
    int x;
    bool operator==(const Simple& b) { return x == b.x; }
  };
  hash_set<Simple> hs;
  EXPECT_DEATH({ hs.Insert(Simple()); }, "Hash not implemented");
}

}  // namespace qian
