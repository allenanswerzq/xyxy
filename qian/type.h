#ifndef QIAN_TYPE_H_
#define QIAN_TYPE_H_

#include "qian/object.h"

namespace qian {

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
  VAL_OBJ,
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    Object* obj;
  } as;
} Value;

// clang-format off

// Converts a qian value into a c++ value.
#define AS_CXX_BOOL(value)    ((value).as.boolean)
#define AS_CXX_NUMBER(value)  ((value).as.number)
#define AS_CXX_OBJ(value)     ((value).as.obj)

// Converts a cxx value into a equalivent qian represtation.
#define QIAN_BOOL(value)      ((Value) {VAL_BOOL,   {.boolean = value}})
#define QIAN_NIL              ((Value) {VAL_NIL,    {.number = 0}})
#define QIAN_NUMBER(value)    ((Value) {VAL_NUMBER, {.number = value}})
#define QIAN_OBJ(value)       ((Value) {VAL_OBJ,    {.obj = (Object*) value}})
#define QIAN_OBJ_TYPE(value)  (AS_CXX_OBJ(value)->Type())

// Check qian value's type
#define IS_QIAN_BOOL(value)   ((value).type == VAL_BOOL)
#define IS_QIAN_NIL(value)    ((value).type == VAL_NIL)
#define IS_QIAN_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_QIAN_BOJ(value)    ((value).type == VAL_OBJ)

// Check object's type
static inline bool is_obj_type(Value val, ObjType type) {
  return IS_QIAN_BOJ(val) && QIAN_OBJ_TYPE(val) == type;
}

#define IS_STRING(value) is_obj_type(value, OBJ_STRING)
#define AS_STRING(value)   ((StringObj*) AS_OBJ(value))
#define AS_CSTRING(value)  (((StringObj*) AS_OBJ(value))->raw)

// clang-format on

inline bool IsEqual(Value a, Value b) {
  if (a.type != b.type) {
    return false;
  }
  if (a.type == VAL_BOOL) {
    return AS_CXX_BOOL(a) == AS_CXX_BOOL(b);
  } else if (a.type == VAL_NIL) {
    return true;
  } else if (a.type == VAL_NUMBER) {
    return AS_CXX_NUMBER(a) == AS_CXX_NUMBER(b);
  } else {
    return false;
  }
}

inline bool IsFalsey(Value value) {
  return IS_QIAN_NIL(value) || (IS_QIAN_BOOL(value) && !AS_CXX_BOOL(value));
}

inline bool operator==(const Value& a, const Value& b) { return IsEqual(a, b); }

inline bool operator!=(const Value& a, const Value& b) { return !(a == b); }

}  // namespace qian

#endif  // QIAN_TYPE_H_
