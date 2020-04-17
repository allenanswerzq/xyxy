#ifndef XYXY_HASH_TABLE_H_
#define XYXY_HASH_TABLE_H_

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
      table_[i] = std::make_unique<List<KeyType>>();
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

  bool Insert(const K& key) { return Insert(std::make_pair(key, false)); }

  // Returns true if an insertion took place.
  bool Insert(const KeyType& key) {
    auto val = key.second;
    if (Find(key.first, &val, /*change=*/true)) {
      return false;
    }
    int slot = hash_.Hash(key.first) % kMaxSlot;
    table_[slot]->AppendTail(key);
    return true;
  }

  // int val;
  // bool ok = ht.Find("foo", &val);
  // if (ok) {
  //   cout << "Found value: " << val << '\n';
  // }
  bool Find(const K& key, V* val = nullptr, bool set = false) const {
    int32 slot = hash_.Hash(key) % kMaxSlot;
    assert(table_[slot]);
    return Find(table_[slot].get(), key, val, set);
  }

  bool Find(List<KeyType>* li, const K& key, V* val, bool set = false) const {
    auto root = li->GetHead();
    while (root) {
      if (root->value.first == key) {
        if (val) {
          if (set) {
            root->value.second = *val;
          }
          else {
            *val = root->value.second;
          }
        }
        return true;
      }
      root = root->next;
    }
    return false;
  }

 private:
  std::vector<std::unique_ptr<List<KeyType>>> table_;
  Hasher hash_;
};

// Define hash_set.
template <class T>
using hash_set = hash_table<T, bool>;
}  // namespace xyxy

#endif  // XYXY_HASH_TABLE_H_
