#ifndef QIAN_UTIL_H_
#define QIAN_UTIL_H_

namespace qian {
template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
  static_assert(!std::is_array<T>::value, "array types are unsupported");
  static_assert(std::is_object<T>::value, "non-object types are unsupported");
  return std::unique_ptr<T>(ptr);
}
}  // namespace qian

#endif  // QIAN_UTIL_H_
