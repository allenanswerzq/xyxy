#ifndef QIAN_BASE_H_
#define QIAN_BASE_H_

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
#define QI_ATTRIBUTE_NORETURN __attribute__((noreturn))
#define QI_ATTRIBUTE_NOINLINE __attribute__((noinline))
#define QI_ATTRIBUTE_UNUSED __attribute__((unused))
#define QI_ATTRIBUTE_COLD __attribute__((cold))
#define QI_PACKED __attribute__((packed))
#define QI_MUST_USE_RESULT __attribute__((warn_unused_result))
#define QI_PRINQI_ATTRIBUTE(string_index, first_to_check) \
  __attribute__((__format__(__printf__, string_index, first_to_check)))
#define QI_SCANF_ATTRIBUTE(string_index, first_to_check) \
  __attribute__((__format__(__scanf__, string_index, first_to_check)))

#else
// Non-GCC equivalents
#define QI_ATTRIBUTE_NORETURN
#define QI_ATTRIBUTE_NOINLINE
#define QI_ATTRIBUTE_UNUSED
#define QI_ATTRIBUTE_COLD
#define QI_MUST_USE_RESULT
#define QI_PACKED
#define QI_PRINQI_ATTRIBUTE(string_index, first_to_check)
#define QI_SCANF_ATTRIBUTE(string_index, first_to_check)
#endif

#endif  // QIAN_BASE_H_
