#ifndef QIAN_OBJECT_H_
#define QIAN_OBJECT_H_

#include "qian/type.h"
#include "qian/vector.h"

namespace qian {

typedef enum {
  OBJ_STRING,

} ObjType;

class Object;

static Vector<Object*>* Collecter() {
  return new Vector<Object*>();
}

class Object {
 public:
  Object(ObjType type) : type_(type) {}
  ObjType Type() { return type_; }

 private:
  ObjType type_;
};

class StringObj : Object {
 public:
  static StringObj* Create(const string& str) {
    auto obj = new StringObj();
    obj->str_ = str;
    Collecter()->Write(obj);
    return obj;
  }

 private:
  StringObj() : Object(OBJ_STRING) {}
  string str_;
};


}  // namespace qian

#endif  // QIAN_OBJECTH_
