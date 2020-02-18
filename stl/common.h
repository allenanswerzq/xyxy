#ifndef QIAN_TYPE_TRAITS_H_
#define QIAN_TYPE_TRAITS_H_

#include <cassert>
#include <memory>

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

namespace stl {

template <typename T>
struct is_pointer {
  enum { value = false };
};

template <typename T>
struct is_pointer<T*> {
  enum { value = true };
};

template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
}

} //

#endif // QIAN_TYPE_TRAITS_H
