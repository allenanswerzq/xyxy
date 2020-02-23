#ifndef QIAN_TYPE_H_
#define QIAN_TYPE_H_

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

  virtual T AsCxx() = 0;

  QValueType Type() const { return type_; }

  QValueType type_;
};

class QBoolean : public QValue<bool> {
 public:
  bool AsCxx() override { return val_; }

  QBoolean() : QValue(QVAL_BOOL) {}

  QBoolean(bool val) : QValue(QVAL_BOOL), val_(val) {}

 private:
  bool val_ = false;
};

class QFloat : public QValue<double> {
 public:
  double AsCxx() override { return val_; }

  QFloat() : QValue(QVAL_FLOAT) {}

  QFloat(double val) : QValue(QVAL_FLOAT), val_(val) {}

 private:
  double val_ = 0;
};

class QNil : public QValue<bool> {
 public:
  bool AsCxx() override { return val_; }

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
