#ifndef QIAN_TYPE_H_
#define QIAN_TYPE_H_

#include "qian/stl/common.h"
#include "qian/stl/memory.h"

namespace qian {

// Define QValue types
typedef enum {
  QVAL_BOOL,
  QVAL_NIL,
  QVAL_FLOAT,
  QVAL_OBJ,
} QValueType;

// Define QObject types
typedef enum {
  QOBJ_OBJ,
  QOBJ_STRING,
} QObjType;

template <class T>
class QValue {
 public:
  QValue(QValueType type) : type_(type) {}

  virtual ~QValue() = default;

  virtual T AsCxx() = 0;

  QValueType Type() const { return type_; }

  QValueType type_;
};

class QBoolean : public QValue<bool> {
 public:
  bool AsCxx() override { return val_; }

  virtual ~QBoolean() = default;

  QBoolean() : QValue(QVAL_BOOL) {}

  QBoolean(bool val) : QValue(QVAL_BOOL), val_(val) {}

 private:
  bool val_ = false;
};

class QFloat : public QValue<double> {
 public:
  double AsCxx() override { return val_; }

  virtual ~QFloat() = default;

  QFloat() : QValue(QVAL_FLOAT) {}

  QFloat(double val) : QValue(QVAL_FLOAT), val_(val) {}

 private:
  double val_ = 0;
};

class QNil : public QValue<bool> {
 public:
  bool AsCxx() override { return val_; }

  virtual ~QNil() = default;

  QNil() : QValue(QVAL_NIL) {}

  QNil(bool val) : QValue(QVAL_NIL), val_(val) {}

 private:
  bool val_ = false;
};

#define DEFINE_TYPE_CHECK(type_name, type_val) \
  template <class T>                           \
  bool Is##type_name(T val) {                  \
    return val.Type() == type_val;             \
  }

#define DEFINE_OBJECT_CHECK(type_name, type_val) \
  template <class T>                             \
  bool Is##type_name(T* val) {                   \
    return val->ObjType() == type_val;           \
  }                                              \
  template <class T>                             \
  bool Is##type_name(T val) {                    \
    return val.ObjType() == type_val;            \
  }

DEFINE_TYPE_CHECK(QBoolean, QVAL_BOOL)
DEFINE_TYPE_CHECK(QFloat, QVAL_FLOAT)
DEFINE_TYPE_CHECK(QNil, QVAL_NIL)

DEFINE_OBJECT_CHECK(QObject, QOBJ_OBJ)
DEFINE_OBJECT_CHECK(QString, QOBJ_STRING)

template <class T>
inline bool is_qvalue_equal(T* a, T* b) {
  assert(a && b);
  if (a->Type() == QVAL_BOOL) {
    return a->AsCxx() == b->AsCxx();
  }
  else if (a->Type() == QVAL_NIL) {
    return true;
  }
  else if (a->Type() == QVAL_FLOAT) {
    return a->AsCxx() == b->AsCxx();
  }
  else {
    return false;
  }
}

template <class T>
inline bool IsQValueEqual(T& a, T& b) {
  return is_qvalue_equal<T>(&a, &b);
}

template <class T>
inline bool IsQValueEqual(Unique_Ptr<T>& a, Unique_Ptr<T>& b) {
  return is_qvalue_equal<T>(a.Get(), b.Get());
}

template <class T>
inline bool IsQValueEqual(T* a, T* b) {
  return is_qvalue_equal<T>(a, b);
}

template <class T>
inline bool is_falsey(T* val) {
  assert(val);
  if (val->Type() == QVAL_NIL) {
    return true;
  }
  else if (val->Type() == QVAL_BOOL) {
    return !val->AsCxx();
  }
  else {
    return false;
  }
}

template <class T>
inline bool IsFalsey(T val) {
  return is_falsey<T>(&val);
}

template <class T>
inline bool IsFalsey(T* val) {
  return is_falsey<T>(val);
}

template <class T>
inline bool IsFalsey(Unique_Ptr<T>& val) {
  return is_falsey<T>(val.Get());
}

template <class T>
bool operator==(T& a, T& b) {
  return IsQValueEqual<T>(a, b);
}

template <class T>
bool operator!=(T& a, T& b) {
  return !(a == b);
}

}  // namespace qian

#endif  // QIAN_TYPE_H_
