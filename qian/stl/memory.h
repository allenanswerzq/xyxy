#ifndef QIAN_UNIQUE_PTR_H_
#define QIAN_UNIQUE_PTR_H_

#include "qian/stl/common.h"

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
class Unique_Ptr {
 public:
  using PointerType = T*;

  Unique_Ptr() : ptr_(nullptr) {}

  ~Unique_Ptr() { Reset(); }

  explicit Unique_Ptr(PointerType p) : ptr_(p) {}

  Unique_Ptr(Unique_Ptr const&) = default;
  Unique_Ptr& operator=(Unique_Ptr const&) = default;

  // Move assignment.
  Unique_Ptr& operator=(Unique_Ptr&& p) {
    Reset(p.Release());
    return *this;
  }

  Unique_Ptr& operator=(std::nullptr_t) {
    Reset();
    return *this;
  }

  T& operator*() const { return *ptr_; }

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

  void swap(Unique_Ptr& u) {
    PointerType tmp = ptr_;
    ptr_ = u->ptr_;
    u->ptr_ = tmp;
  }

 private:
  PointerType ptr_;
  Deleter deleter_;
};

template <typename T>
Unique_Ptr<T> WrapUnique(T* ptr) {
  return Unique_Ptr<T>(ptr);
}

// Do not support array.
template <typename T, typename... Args>
Unique_Ptr<T> MakeUnique(Args&&... args) {
  return Unique_Ptr<T>(new T(std::forward<Args>(args)...));
}

#endif  // QIAN_UNIQUE_PTR_H_
