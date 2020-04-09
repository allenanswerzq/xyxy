#include "xyxy/chunk.h"

#include <memory>

namespace xyxy {

Chunk::Chunk() {}

void Chunk::Write(uint8 byte) { code_.push_back(byte); }

void Chunk::Write(uint8 byte, int ln) {
  Write(byte);
  lines_.push_back(ln);
}

void Chunk::Write(uint8 byte, int idx, Value val) {
  Write(byte);
  Write(idx);
  constants_.push_back(val);
}

void Chunk::Write(uint8 byte, int idx, Value val, int ln) {
  Write(byte, idx, val);
  lines_.push_back(ln);
}

uint8 Chunk::GetByte(int idx) { return code_.at(idx); }

int Chunk::AddConstant(Value val) {
  constants_.push_back(val);
  return (int)constants_.size() - 1;
}

Value Chunk::GetConstant(int idx) {
  assert(idx < constants_.size());
  return constants_.at(idx);
}

}  // namespace xyxy
