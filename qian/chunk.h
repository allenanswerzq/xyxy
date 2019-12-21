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

  void WriteByte(uint8_t byte);
  void WriteChunk(uint8_t byte, uint32_t ln);
  void WriteChunk(uint8_t byte, Value value, uint32_t ln);
  uint8_t GetByte(int index);
  uint8_t GetLine(int index);

  Vector<uint8_t>* Code() { return code_; }
  uint32_t Size() { return code_->Size(); }

  int AddValue(Value value);
  Value GetValue(int index);

 private:
  Vector<uint8_t>* code_;
  Vector<Value>* value_array_;
  Vector<uint32_t>* lines_;
};

} // namespace qian

#endif  // QIAN_CHUNK_H_
