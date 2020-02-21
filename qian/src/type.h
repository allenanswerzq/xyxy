#ifndef QIAN_TYPE_H_
#define QIAN_TYPE_H_

// #include "qian/src/object.h"

namespace qian {

typedef enum {
  QVAL_BOOL,
  QVAL_NIL,
  QVAL_FLOAT,
  QVAL_OBJ,
} QValueType;

class QValue {
 public:
  QValue(QValueType type) : type_(type) {}

  QValueType Type() const { return type_; }

  QValueType type_;
};

class QBoolean : public QValue {
 public:
  bool AsCxx() const { return val_; }

  QBoolean() : QValue(QVAL_BOOL) {}

  QBoolean(bool val) : QValue(QVAL_BOOL), val_(val) {}

 private:
  bool val_ = false;
};

class QFloat : public QValue {
 public:
  double AsCxx() const { return val_; }

  QFloat() : QValue(QVAL_FLOAT) {}

  QFloat(double val) : QValue(QVAL_FLOAT), val_(val) {}

 private:
  double val_ = 0;
};

class QNil : public QValue {
 public:
  bool AsCxx() const { return val_; }

  QNil() : QValue(QVAL_NIL) {}

  QNil(double val) : QValue(QVAL_NIL), val_(val) {}

 private:
  bool val_ = false;
};

#define DEFINE_TYPE_CHECK(type_name, type_val) \
  template <class T>                           \
  bool Is##type_name(T val) {                  \
    return val.Type() == type_val;             \
  }

DEFINE_TYPE_CHECK(QBoolean, QVAL_BOOL)
DEFINE_TYPE_CHECK(QFloat, QVAL_FLOAT)
DEFINE_TYPE_CHECK(QNil, QVAL_NIL)

template <class T, class U>
inline bool IsQValueEqual(T a, U b) {
  if (a.Type() != b.Type()) {
    return false;
  }
  if (a.Type() == QVAL_BOOL) {
    return a.AsCxx() == b.AsCxx();
  }
  else if (a.Type() == QVAL_NIL) {
    return true;
  }
  else if (a.Type() == QVAL_FLOAT) {
    return a.AsCxx() == b.AsCxx();
  }
  else {
    return false;
  }
}

template <class T>
inline bool IsFalsey(T val) {
  if (val.Type() == QVAL_NIL) {
    return true;
  }
  else if (val.Type() == QVAL_BOOL) {
    return !val.AsCxx();
  }
  else {
    return false;
  }
}

template <class T, class U>
bool operator==(const T& a, const U& b) {
  return IsQValueEqual(a, b);
}

template <class T, class U>
bool operator!=(const T& a, const U& b) {
  return !(a == b);
}

}  // namespace qian

#endif  // QIAN_TYPE_H_
