#ifndef QIAN_HASH_TABLE_H_
#define QIAN_HASH_TABLE_H_

#include "qian/stl/common.h"
#include "qian/stl/list.h"
#include "qian/stl/vector.h"

namespace stl {

template <class T>
struct DefaultHasher {
  uint32 Hash(const T& val) const {
    ABORT("Hash not implemented, Please define it.");
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
class HashTable {
 public:
  const int kMaxSlot = 1e6 + 7;

  HashTable() { table_.Reserve(kMaxSlot); }

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
    table_[slot].AppendTail(key);
    return ret;
  }

  // int val;
  // bool ok = ht.Find("foo", &val);
  // if (ok) {
  //   cout << "Found value: " << val << '\n';
  // }
  bool Find(const K& key, V* val = nullptr) const {
    int32 slot = hash_.Hash(key) % kMaxSlot;
    return Find(table_[slot], key, val);
  }

  bool Find(const List<KeyType>& li, const K& key, V* val) const {
    auto root = li.GetHead();
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
  Vector<List<KeyType>> table_;
  Hasher hash_;
};

// Define HashSet.
template <class T>
using HashSet = HashTable<T, bool>;
}  // namespace stl

#endif  // QIAN_HASH_TABLE_H_
