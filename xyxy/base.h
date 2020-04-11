#ifndef XYXY_BASE_H_
#define XYXY_BASE_H_

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

// #define NDEBUG

#define STACK_SIZE 256

using std::string;

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// Compiler attributes
#if (defined(__GNUC__) || defined(__APPLE__)) && !defined(SWIG)
// Compiler supports GCC-style attributes
#define XY_ATTRIBUTE_NORETURN __attribute__((noreturn))
#define XY_ATTRIBUTE_NOINLINE __attribute__((noinline))
#define XY_ATTRIBUTE_UNUSED __attribute__((unused))
#define XY_ATTRIBUTE_COLD __attribute__((cold))
#define XY_PACKED __attribute__((packed))
#define XY_MUST_USE_RESULT __attribute__((warn_unused_result))
#define XY_PRINXY_ATTRIBUTE(string_index, first_to_check) \
  __attribute__((__format__(__printf__, string_index, first_to_check)))
#define XY_SCANF_ATTRIBUTE(string_index, first_to_check) \
  __attribute__((__format__(__scanf__, string_index, first_to_check)))

#else
// Non-GCC equivalents
#define XY_ATTRIBUTE_NORETURN
#define XY_ATTRIBUTE_NOINLINE
#define XY_ATTRIBUTE_UNUSED
#define XY_ATTRIBUTE_COLD
#define XY_MUST_USE_RESULT
#define XY_PACKED
#define XY_PRINXY_ATTRIBUTE(string_index, first_to_check)
#define XY_SCANF_ATTRIBUTE(string_index, first_to_check)
#endif

#endif  // XYXY_BASE_H_
