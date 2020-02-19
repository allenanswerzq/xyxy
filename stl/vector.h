#ifndef QIAN_VECTOR_H_
#define QIAN_VECTOR_H_

#include "stl/common.h"

namespace stl {

template <class T>
class Vector {
 public:
  Vector();
  virtual ~Vector();

  void Write(T value);
  T Get(int index);
  uint32 Size() { return count_; }
  T* Address() { return container_; }

 private:
  void grow_vector();

  uint32 count_;
  uint32 capacity_;
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
    delete[] container_;
    container_ = nullptr;
  }
}

template <class T>
void Vector<T>::grow_vector() {
  int new_size = (capacity_ < 8 ? 8 : 2 * capacity_);
  T* ptr = new T[new_size];
  assert(ptr);
  for (int i = 0; i < capacity_; i++) {
    ptr[i] = Get(i);
  }
  if (container_) {
    delete container_;
  }
  container_ = ptr;
  capacity_ = new_size;
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
  assert(container_ && index < Size());
  return container_[index];
}
}

#endif  // QIAN_VECTOR_H_
