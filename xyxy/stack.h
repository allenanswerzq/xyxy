#ifndef XYXY_STACK_H_
#define XYXY_STACK_H_

#include <glog/logging.h>

#include "xyxy/base.h"

namespace xyxy {

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

  T Top() {
    assert(!Empty());
    return *(top_ - 1);
  }

 private:
  T* stk_;
  T* top_;
};

}  // namespace xyxy

#endif  // XYXY_STACK_H_
