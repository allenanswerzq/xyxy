#ifndef QIAN_VECTOR_H_
#define QIAN_VECTOR_H_

template <class T>
class Vector {
 public:
  Vector();
  virtual ~Vector();
  void Write(T value);
  T Get(int index);
  uint32_t Size() { return count_; }

 private:
  void grow_vector();

  int count_;
  int capacity_;
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
  uint32_t size = (capacity_ < 8 ? 8 : 2 * capacity_);
  capacity_ = size;
  void* mem = realloc(container_, capacity_);
  container_ = static_cast<uint8_t*>(mem);
}

template <class T>
void Vector<T>::Write(T value) {
  if (capacity_ < count_ + 1) {
    grow_vector();
  }
  container_[count_++] = value;
}

template <class T>
T Vector<T>::Get(int index) {
  assert(container_ && index < capacity_);
  return container_[index];
}

#endif // QIAN_VECTOR_H_