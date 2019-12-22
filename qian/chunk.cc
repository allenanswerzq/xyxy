#include "chunk.h"

namespace qian {

Chunk::Chunk() {
  code_ = new Vector<uint8_t>();
  value_array_  = new Vector<Value>();
  lines_ = new Vector<int>();
}

Chunk::~Chunk() {
  if (code_) {
    delete code_;
    code_ = nullptr;
  }
  if (value_array_) {
    delete value_array_;
    value_array_ = nullptr;
  }
}

void Chunk::WriteByte(uint8_t byte) {
  assert(code_);
  code_->Write(byte);
}

void Chunk::WriteChunk(uint8_t byte, int ln) {
  assert(code_ && lines_);
  WriteByte(byte);
  lines_->Write(ln);
}

void Chunk::WriteChunk(uint8_t byte, int index, Value value) {
  assert(code_ && lines_ && value_array_);
  WriteByte(byte);
  WriteByte(index);
  value_array_->Write(value);
}

void Chunk::WriteChunk(uint8_t byte, int index, Value value, int ln) {
  assert(code_ && lines_ && value_array_);
  WriteChunk(byte, index, value);
  lines_->Write(ln);
}

uint8_t Chunk::GetByte(int index) {
  assert(code_);
  return code_->Get(index);
}

int Chunk::AddValue(Value value) {
  assert(value_array_);
  value_array_->Write(value);
  return value_array_->Size() - 1;
}

Value Chunk::GetValue(int index) {
  assert(index < value_array_->Size());
  return value_array_->Get(index);
}

} // namespace qian