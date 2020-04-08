#include "xyxy/chunk.h"

#include <memory>

namespace xyxy {

Chunk::Chunk() {
  code_ = std::make_unique<std::vector<uint8>>();
  lines_ = std::make_unique<std::vector<int>>();
  constants_ = std::make_unique<std::vector<Value>>();
}

void Chunk::Write(uint8 byte) {
  assert(code_);
  code_->push_back(byte);
}

void Chunk::Write(uint8 byte, int ln) {
  assert(code_ && lines_);
  Write(byte);
  lines_->push_back(ln);
}

void Chunk::Write(uint8 byte, int idx, Value val) {
  assert(code_ && lines_ && constants_);
  Write(byte);
  Write(idx);
  constants_->push_back(val);
}

void Chunk::Write(uint8 byte, int idx, Value val, int ln) {
  assert(code_ && lines_ && constants_);
  Write(byte, idx, val);
  lines_->push_back(ln);
}

uint8 Chunk::GetByte(int idx) {
  assert(code_);
  return code_->at(idx);
}

int Chunk::AddConstant(Value val) {
  assert(constants_);
  constants_->push_back(val);
  return (int)constants_->size() - 1;
}

Value Chunk::GetConstant(int idx) {
  assert(idx < constants_->size());
  return constants_->at(idx);
}

}  // namespace xyxy
