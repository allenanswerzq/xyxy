#ifndef QIAN_STACK_H_
#define QIAN_STACK_H_

namespace qian {

template <class T, uint32_t N>
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

  bool Full() {
    return top_ == stk_ + N;
  }

  bool Empty() {
    return top_ == stk_;
  }

  void Push(const T& value) {
    assert(!Full());
    *top_++ = value;
  }

  T Pop() {
    assert(!Empty());
    return *(--top_);
  }

  // Dump stack content.
  void DebugInfo() {
    for (T* idx = stk_; idx < top_; idx++) {
      printf("[ %g ]\n", *idx);
    }
  }

 private:
  T* stk_;
  T* top_;
};

} // namespace qian

#endif  // QIAN_STACK_H_
