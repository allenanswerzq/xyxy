#ifndef QIAN_STACK_H_
#define QIAN_STACK_H_

#include "stl/common.h"

namespace stl {

template <class T, int N>
class Stack {
 public:
  Stack() {
    stk_ = new T[N];
    top_ = stk_;
  }

  ~Stack() {
    if (stk_) {
      delete stk_;
      stk_ = nullptr;
      top_ = nullptr;
    }
  }

  bool Full() { return top_ == stk_ + N; }

  bool Empty() { return top_ == stk_; }

  void Push(const T& value) {
    assert(!Full());
    *top_++ = value;
  }

  T Pop() {
    assert(!Empty());
    return *(--top_);
  }

  T Top() { return *top_; }

 private:
  T* stk_;
  T* top_;
};

}  // namespace stl

#endif  // QIAN_STACK_H_
