#ifndef QIAN_CHUNK_H_
#define QIAN_CHUNK_H_

#include <vector>

#include "qian/base.h"
#include "qian/type.h"

namespace qian {

// A class used to represent bytecode.
class Chunk {
 public:
  Chunk();
  virtual ~Chunk() = default;

  void Write(uint8 byte);
  // Writes a bytecode and associate a line number with it.
  void Write(uint8 byte, int ln);
  // Writes a bytecode and associate a value with it.
  void Write(uint8 byte, int index, Value val);
  void Write(uint8 byte, int index, Value val, int ln);

  uint8 GetByte(int index);
  uint8 GetLine(int index);

  std::unique_ptr<std::vector<uint8>>& Code() { return code_; }
  int Size() { return code_->size(); }

  int AddConstant(Value val);
  Value GetConstant(int index);

 private:
  // Store bytecode.
  std::unique_ptr<std::vector<uint8>> code_;
  // Constant pool used to store constant value, lookup by index.
  std::unique_ptr<std::vector<Value>> constants_;
  // Store the line number of source code.
  std::unique_ptr<std::vector<int>> lines_;
};

}  // namespace qian

#endif  // QIAN_CHUNK_H_
