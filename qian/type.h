#ifndef QIAN_TYPE_H_
#define QIAN_TYPE_H_

namespace qian {

typedef enum { VAL_BOOL, VAL_NIL, VAL_NUMBER } ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
  } as;
} Value;

// Converts a qian value into a c++ value.
#define AS_CXX_BOOL(value) ((value).as.boolean)
#define AS_CXX_NUMBER(value) ((value).as.number)

// Converts a cxx value into a equalivent qian represtation.
#define QIAN_BOOL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define QIAN_NIL ((Value){VAL_NIL, {.number = 0}})
#define QIAN_NUMBER(value) ((Value){VAL_NUMBER, {.number = value}})

// Check qian value's type
#define IS_QIAN_BOOL(value) ((value).type == VAL_BOOL)
#define IS_QIAN_NIL(value) ((value).type == VAL_NIL)
#define IS_QIAN_NUMBER(value) ((value).type == VAL_NUMBER)

inline bool is_equal(Value a, Value b) {
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

inline bool is_falsey(Value value) {
  return IS_QIAN_NIL(value) || (IS_QIAN_BOOL(value) && !AS_CXX_BOOL(value));
}

inline bool operator==(const Value& a, const Value& b) {
  return is_equal(a, b);
}

inline bool operator!=(const Value& a, const Value& b) { return !(a == b); }

}  // namespace qian
#endif  // QIAN_TYPE_H_
