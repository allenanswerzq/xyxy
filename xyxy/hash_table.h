#ifndef QIAN_HASH_TABLE_H_
#define QIAN_HASH_TABLE_H_

#include <cassert>
#include <memory>
#include <vector>

#include "xyxy/list.h"

namespace xyxy {

#define assertm(exp, msg) assert(((void)msg, exp))

template <class T>
struct DefaultHasher {
  uint32 Hash(const T& val) const {
    assertm(0, "Hash not implemented, Please define it.");
    (void)val;
    return -1;
  }
};

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
template <>
struct DefaultHasher<std::string> {
  uint32 Hash(const std::string& s) const {
    uint32 hash = 2166136261u;
    for (int i = 0; i < s.size(); i++) {
      hash ^= s[i];
      hash *= 16777619;
    }
    return hash;
  }
};

template <class K, class V, class Hasher = DefaultHasher<K>>
class hash_table {
 public:
  const int kMaxSlot = 1e3;

  hash_table() {
    table_.resize(kMaxSlot);
    for (int i = 0; i < kMaxSlot; i++) {
      table_[i] = std::make_shared<List<KeyType>>();
      assert(table_[i].get());
    }
  }

  using KeyType = std::pair<K, V>;

  //
  // ht["foo"] = bar;
  // T& operator[](const K& key) {
  //   V val;
  //   if (Find(key, &val)) {
  //     return val;
  //   }
  //   else {
  //     Insert({key, val});
  //     Find(key, &val);
  //     return val;
  //   }
  // }
  // const T& operator[](int n) const { return Get(n); }

  bool Insert(const K& key, const V& val) {
    return Insert(make_pair(key, val));
  }

  void Insert(const K& key) { Insert(std::make_pair(key, false)); }

  bool Insert(const KeyType& key) {
    bool ret = false;
    if (Find(key.first)) {
      ret = true;
    }
    int slot = hash_.Hash(key.first) % kMaxSlot;
    table_[slot]->AppendTail(key);
    return ret;
  }

  // int val;
  // bool ok = ht.Find("foo", &val);
  // if (ok) {
  //   cout << "Found value: " << val << '\n';
  // }
  bool Find(const K& key, V* val = nullptr) const {
    int32 slot = hash_.Hash(key) % kMaxSlot;
    assert(table_[slot]);
    return Find(table_[slot], key, val);
  }

  bool Find(std::shared_ptr<List<KeyType>> li, const K& key, V* val) const {
    auto root = li->GetHead();
    while (root) {
      if (root->value.first == key) {
        if (val) {
          *val = root->value.second;
        }
        return true;
      }
      root = root->next;
    }
    return false;
  }

 private:
  std::vector<std::shared_ptr<List<KeyType>>> table_;
  Hasher hash_;
};

// Define hash_set.
template <class T>
using hash_set = hash_table<T, bool>;
}  // namespace xyxy

#endif  // QIAN_HASH_TABLE_H_
