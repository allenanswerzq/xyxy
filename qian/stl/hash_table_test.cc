#include "qian/stl/hash_table.h"

#include <string>

#include "gtest/gtest.h"

namespace qian {

TEST(String, HashTableTest) {
  HashTable<std::string, int> ht;
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

TEST(String, HashSetTest) {
  HashSet<std::string> hs;
  hs.Insert("foo");
  hs.Insert("bar");
  EXPECT_TRUE(hs.Find("foo"));
  EXPECT_TRUE(hs.Find("bar"));
  EXPECT_FALSE(hs.Find("tar"));
}

TEST(NoDefineHash, HashSetTest) {
  struct Simple {
    int x;
    bool operator==(const Simple& b) { return x == b.x; }
  };
  HashSet<Simple> hs;
  EXPECT_DEATH({ hs.Insert(Simple()); }, "Hash not implemented");
}

}  // namespace qian
