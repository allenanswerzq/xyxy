#ifndef QIAN_VECTOR_H_
#define QIAN_VECTOR_H_

#include "qian/stl/common.h"

// TODO(zq7): initializer_list and iterator support.
template <class T>
class Vector {
 public:
  Vector();
  virtual ~Vector();

  Vector(int n) {
    begin_ = 0;
    end_ = n;
    capacity_ = n;
    data_ = new T[n];
  }

  // Delete copy constructs.
  Vector(const Vector& v) = delete;
  Vector<T>& operator=(const Vector& v) = delete;

  void Write(T value);

  void Clean() {
    begin_ = 0;
    end_ = 0;
    capacity_ = 0;
    if (data_) {
      delete data_;
    }
  }

  T& Get(int index);
  const T& Get(int index) const;
  T& operator[](int n) { return Get(n); }
  const T& operator[](int n) const { return Get(n); }

  uint32 Size() const { return end_ - begin_; }

  // Move assignment.
  Vector<T>& operator=(Vector&& v) {
    begin_ = v->begin_;
    end_ = v->end_;
    capacity_ = v->capacity_;
    data_ = v->data_;
    // Reset v.
    v->begin_ = 0;
    v->end_ = 0;
    v->capacity_ = 0;
    v->data_ = nullptr;
  }

  void Reserve(int n) {
    capacity_ = n;
    begin_ = 0;
    end_ = n;
    if (data_) {
      delete data_;
    }
    data_ = new T[n];
  }

 private:
  void grow_vector();

  uint32 begin_;
  uint32 end_;
  uint32 capacity_;
  T* data_;
};

template <class T>
Vector<T>::Vector() {
  capacity_ = 0;
  begin_ = 0;
  end_ = 0;
  data_ = nullptr;
}

template <class T>
Vector<T>::~Vector() {
  if (data_) {
    delete[] data_;
    data_ = nullptr;
  }
}

template <class T>
void Vector<T>::grow_vector() {
  int new_size = (capacity_ < 8 ? 8 : 2 * capacity_);
  T* ptr = new T[new_size];
  assert(ptr);
  for (int i = 0; i < capacity_; i++) {
    ptr[i] = std::move(Get(i));
  }
  if (data_) {
    delete data_;
  }
  data_ = ptr;
  capacity_ = new_size;
}

template <class T>
void Vector<T>::Write(T val) {
  if (Size() + 1 > capacity_) {
    grow_vector();
  }
  data_[end_++] = std::move(val);
}

template <class T>
T& Vector<T>::Get(int index) {
  assert(data_ && index < Size());
  return data_[index];
}

template <class T>
const T& Vector<T>::Get(int index) const {
  assert(data_ && index < Size());
  return data_[index];
}

#endif  // QIAN_VECTOR_H_
