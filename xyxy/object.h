#ifndef XYXY_OBJECT_H_
#define XYXY_OBJECT_H_

#include <memory>
#include <string>
#include <vector>

#include "xyxy/chunk.h"

namespace xyxy {

// Forward declaration.
class Object;
class Chunk;

// Temporary garbage collector to free all objects, later when we
// add a real garbage collector, this will be removed.
static std::vector<Object*>* Collector() {
  static std::vector<Object*>* collector = new std::vector<Object*>;
  return collector;
}

enum class ObjType {
  OBJ_UNDEF,
  OBJ_STRING,
  OBJ_FUNCTION,
};

class Object {
 public:
  Object(ObjType type) : type_(type) {}

  virtual ~Object() = default;

  ObjType Type() const { return type_; }

  bool IsString() { return type_ == ObjType::OBJ_STRING; }
  bool IsFunction() { return type_ == ObjType::OBJ_FUNCTION; }

  virtual std::string ToString() = 0;

 private:
  ObjType type_;
};

class ObjString : public Object {
 public:
  ObjString(const std::string& str) : Object(ObjType::OBJ_STRING), str_(str) {
    Collector()->push_back(this);
  }

  std::string ToString() override { return str_; }

 private:
  std::string str_;
};

enum FunctionType {
  FUNCTION_UNDEF,
  FUNCTION_FUNC,
  FUNCTION_SCRIPT,
};

class ObjFunction : public Object {
 public:
  ObjFunction(const std::string& name, FunctionType type)
      : Object(ObjType::OBJ_FUNCTION), name_(name), type_(type) {
    Collector()->push_back(this);
    chunk_ = std::make_unique<Chunk>();
  }

  std::string ToString() override { return name_; }

  Chunk* GetChunk() { return chunk_.get(); }
  void AddArg() { args_num_++; }
  int ArgNum() { return args_num_; }
  FunctionType Type() { return type_; }

  // Function name.
  std::string name_;
  // Function type.
  FunctionType type_;
  // The number of arguments this function has.
  int args_num_ = 0;
  // Bytecode of this function.
  std::unique_ptr<Chunk> chunk_;
};

}  // namespace xyxy

#endif  // XYXY_OBJECTH_
