#ifndef QIAN_VECTOR_H_
#define QIAN_VECTOR_H_

#include "base.h"

namespace qian {

template <class T>
class Vector {
 public:
  Vector();
  virtual ~Vector();

  void Write(T value);
  T Get(int index);
  uint32_t Size() { return count_; }
  T* Address() { return container_; }

 private:
  void grow_vector();

  uint32_t count_;
  uint32_t capacity_;
  T* container_;
};

template <class T>
Vector<T>::Vector() {
  capacity_ = 0;
  count_ = 0;
  container_ = nullptr;
}

template <class T>
Vector<T>::~Vector() {
  if (container_) {
    free(container_);
    container_ = nullptr;
  }
}

template <class T>
void Vector<T>::grow_vector() {
  capacity_ = (capacity_ < 8 ? 8 : 2 * capacity_);
  void* mem = realloc(container_, capacity_ * sizeof(T));
  assert(mem);
  container_ = static_cast<T*>(mem);
}

template <class T>
void Vector<T>::Write(T value) {
  if (count_ + 1 > capacity_) {
    grow_vector();
  }
  container_[count_++] = value;
}

template <class T>
T Vector<T>::Get(int index) {
  assert(container_ && index < capacity_);
  return container_[index];
}
}  // namespace qian

#endif // QIAN_VECTOR_H_
