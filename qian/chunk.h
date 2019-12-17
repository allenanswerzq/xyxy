#ifndef QIAN_CHUNK_H_
#define QIAN_CHUNK_H_

#include "base.h"

class Chunk {
 public:
  Chunk();
  virtual ~Chunk();
  void WriteChunk(uint8_t byte);
  uint8_t GetCode(int p);
  uint32_t Size() { return count_; }

  void grow_chunk();

 private:
  int count_;
  int capacity_;
  uint8_t* code_;
};

Chunk::Chunk() {
  capacity_ = 0;
  count_ = 0;
  code_ = nullptr;
}

Chunk::~Chunk() {
  if (code_) {
    free(code_);
    code_ = nullptr;
  }
}

void Chunk::grow_chunk() {
  uint32_t size = (capacity_ < 8 ? 8 : 2 * capacity_);
  capacity_ = size;
  void* mem = realloc(code_, capacity_);
  code_ = static_cast<uint8_t*>(mem);
}

void Chunk::WriteChunk(uint8_t byte) {
  if (capacity_ < count_ + 1) {
    grow_chunk();
  }
  code_[count_++] = byte;
}

uint8_t Chunk::GetCode(int p) {
  assert(code_ && p < capacity_);
  return code_[p];
}

#endif  // QIAN_CHUNK_H_
