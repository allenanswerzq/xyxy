#ifndef XYXY_TYPE_H_
#define XYXY_TYPE_H_

#include "xyxy/object.h"

namespace xyxy {

enum class ValueType {
  VAL_BOOL,
  VAL_NIL,
  VAL_FLOAT,
  VAL_OBJ,
};

class Value {
 private:
  // Denotes the value type.
  ValueType type_;

  // An union type stores the real value for each kind.
  union {
    bool boolean;
    double number;
    // NOTE: this class only sort of serving as a `Object` container.
    Object* obj;  // not owned
  } as_;

 public:
  // By default, constructs a Nil value.
  Value() : type_(ValueType::VAL_NIL) {}

  Value(bool b) : type_(ValueType::VAL_BOOL) { as_.boolean = b; }

  Value(double n) : type_(ValueType::VAL_FLOAT) { as_.number = n; }

  Value(int n) : type_(ValueType::VAL_FLOAT) { as_.number = n; }

  Value(Object* o) : type_(ValueType::VAL_OBJ) { as_.obj = o; }

  ValueType Type() const { return type_; }

  ObjType ObjectType() {
    assert(IsObject());
    return as_.obj->Type();
  }

  bool IsBool() { return type_ == ValueType::VAL_BOOL; }
  bool IsFloat() { return type_ == ValueType::VAL_FLOAT; }
  bool IsObject() { return type_ == ValueType::VAL_OBJ; }
  bool IsNil() { return type_ == ValueType::VAL_NIL; }
  bool IsFalsey() { return IsNil() || (IsBool() && !AsBool()); }

  bool AsBool() {
    assert(IsBool());
    return as_.boolean;
  }

  bool AsNil() {
    assert(IsNil());
    return false;
  }

  double AsFloat() {
    assert(IsFloat());
    return as_.number;
  }

  Object* AsRawObject() {
    assert(IsObject());
    return as_.obj;
  }

  Value(ObjString* p) : type_(ValueType::VAL_OBJ) {
    as_.obj = reinterpret_cast<Object*>(p);
  }

  bool IsString() {
    assert(IsObject());
    return AsRawObject()->IsString();
  }

  ObjString* AsString() {
    assert(IsString());
    return reinterpret_cast<ObjString*>(AsRawObject());
  }

  std::string ToString() {
    if (IsBool()) {
      return std::to_string(AsBool());
    } else if (IsFloat()) {
      return std::to_string(AsFloat());
    } else if (IsNil()) {
      return "Nill";
    } else if (IsObject()) {
      return AsRawObject()->ToString();
    } else {
      // Expect never reach here.
      assert(false);
      return "";
    }
  }
};

inline bool is_equal(Value a, Value b) {
  if (a.Type() != b.Type()) {
    return false;
  }
  if (a.Type() == ValueType::VAL_BOOL) {
    return a.AsBool() == b.AsBool();
  } else if (a.Type() == ValueType::VAL_NIL) {
    return true;
  } else if (a.Type() == ValueType::VAL_FLOAT) {
    return a.AsFloat() == b.AsFloat();
  } else {
    return false;
  }
}

// Define NILL.
#define NILL Value()
#define XYXY_NIL Value()

// NOTE: Use inline here otherwise need to put function implementation into a
// .cc file.
inline bool operator==(const Value& a, const Value& b) {
  return is_equal(a, b);
}

inline bool operator!=(const Value& a, const Value& b) { return !(a == b); }

}  // namespace xyxy

#endif  // XYXY_TYPE_H_
