#ifndef QIAN_CHUNK_H_
#define QIAN_CHUNK_H_

#include "base.h"
#include "vector.h"

namespace qian {
class Chunk {
 public:
  Chunk();
  virtual ~Chunk();

  void WriteChunk(uint8_t byte);
  uint8_t GetCode(int p);

  uint32_t Size() { return code_->Size(); }

  int AddConstant(Value value);

 private:
  Vector<uint8_t>* code_;
  Vector<Value>* value_array_;
  Vector<int>* lines_;
};

Chunk::Chunk() {
  code_ = new Vector<uint8_t>();
  value_array_  = new Vector<Value>();
}

Chunk::~Chunk() {
  if (code_) {
    free(code_);
    code_ = nullptr;
  }
  if (value_array_) {
    free(value_array_);
    value_array_ = nullptr;
  }
}

void Chunk::WriteChunk(uint8_t byte) {
  assert(code_);
  code_->Write(byte);
}

uint8_t Chunk::GetCode(int index) {
  assert(code_);
  return code_->Get(index);
}

int Chunk::AddConstant(Value value) {
  assert(value_array_);
  value_array_->Write(value);
  return value_array_->Size() - 1;
}
}

#endif  // QIAN_CHUNK_H_
