#ifndef XYXY_OBJECT_H_
#define XYXY_OBJECT_H_

#include <string>
#include <vector>

namespace xyxy {

enum class ObjType {
  OBJ_STRING,

};

class Object {
 public:
  Object(ObjType type) : type_(type) {}

  virtual ~Object() = default;

  ObjType Type() const { return type_; }

  bool IsString() { return type_ == ObjType::OBJ_STRING; }

  // Must inherted by all subclasses.
  virtual std::string ToString() = 0;

 private:
  ObjType type_;
};

class ObjString : Object {
 public:
  ObjString(const std::string& str) : Object(ObjType::OBJ_STRING), str_(str) {}

  std::string ToString() override { return str_; }

 private:
  std::string str_;
};

// NOTE: Use inline here otherwise need to put function implementation into a
// .cc file.
// Temporary garbage collector to free all objects, later when we
// add a real garbage collector, this will be removed.
inline std::vector<Object*>* Collector() {
  static std::vector<Object*>* collector = new std::vector<Object*>;
  return collector;
}

}  // namespace xyxy

#endif  // XYXY_OBJECTH_
