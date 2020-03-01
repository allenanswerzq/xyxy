#ifndef QIAN_UNIQUE_PTR_H_
#define QIAN_UNIQUE_PTR_H_

#include "qian/stl/common.h"

namespace qian {
template <class T>
struct DefaultDeleter {
  void Delete(T* ptr) const { delete ptr; }
};

template <class T>
struct DefaultDeleter<T[]> {
  template <class U>
  void Delete(U* ptr) const {
    delete[] ptr;
  }
};

template <class T, class Deleter = DefaultDeleter<T>>
class unique_ptr {
 public:
  using PointerType = T*;

  unique_ptr() : ptr_(nullptr) {}

  virtual ~unique_ptr() { Reset(); }

  explicit unique_ptr(PointerType p) : ptr_(p) {}

  unique_ptr(unique_ptr const&) = default;
  unique_ptr& operator=(unique_ptr const&) = default;

  // Move assignment.
  unique_ptr& operator=(unique_ptr&& p) {
    Reset(p.Release());
    return *this;
  }

  unique_ptr& operator=(std::nullptr_t) {
    Reset();
    return *this;
  }

  T& operator*() const { return *ptr_; }

  PointerType Get() { return ptr_; }
  PointerType operator->() const { return ptr_; }

  explicit operator bool() const { return ptr_ != nullptr; }

  PointerType Release() {
    PointerType t = ptr_;
    ptr_ = nullptr;
    return t;
  }

  void Reset(PointerType p = nullptr) {
    PointerType tmp = ptr_;
    ptr_ = p;
    if (tmp) {
      deleter_.Delete(tmp);
    }
  }

  void swap(unique_ptr& u) {
    PointerType tmp = ptr_;
    ptr_ = u->ptr_;
    u->ptr_ = tmp;
  }

 private:
  PointerType ptr_;
  Deleter deleter_;
};

template <typename T>
unique_ptr<T> WrapUnique(T* ptr) {
  return unique_ptr<T>(ptr);
}

// Do not support array.
template <typename T, typename... Args>
unique_ptr<T> MakeUnique(Args&&... args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace qian
#endif  // QIAN_UNIQUE_PTR_H_
