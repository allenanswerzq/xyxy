#ifndef QIAN_CHUNK_H_
#define QIAN_CHUNK_H_

#include "base.h"
#include "vector.h"

namespace qian {

// A class used to represent bytecode.
class Chunk {
 public:
  Chunk();
  virtual ~Chunk();

  void WriteByte(uint8 byte);
  void WriteChunk(uint8 byte, int ln);
  void WriteChunk(uint8 byte, int index, Value value);
  void WriteChunk(uint8 byte, int index, Value value, int ln);
  uint8 GetByte(int index);
  uint8 GetLine(int index);

  Vector<uint8>* Code() { return code_; }
  uint32_t Size() { return code_->Size(); }

  int AddValue(Value value);
  Value GetValue(int index);

 private:
  Vector<uint8>* code_;
  Vector<Value>* value_array_;
  Vector<int>* lines_;
};

} // namespace qian

#endif  // QIAN_CHUNK_H_
