#ifndef QIAN_DEBUG_H_
#define QIAN_DEBUG_H_

#include "chunk.h"
#include "inst.h"

namespace qian {
class Disambler {
 public:
  Disambler(Chunk* chunk, const string& name);
  virtual ~Disambler() {}

  void DisambleChunk();

  int DisambleInst(int offset);

 private:
  Chunk* chunk_; // not owned
  string name_;
};

Disambler::Disambler(Chunk* chunk, const string& name) {
  chunk_ = chunk;
  name_ = name;
}

void Disambler::DisambleChunk() {
  printf("== %s ==\n", name_.c_str());
  for (int offset = 0; offset < chunk_->Size(); ) {
    offset = DisambleInst(offset);
  }
}

int Disambler::DisambleInst(int offset) {
  printf("%04d ", offset);
  Inst* inst = GetInst(chunk_, offset);
  inst->DebugInfo();
  offset += inst->Length();
  return offset;
}

void DisambleChunk(Chunk* chunk, const string& name) {
  Disambler dis(chunk, name);
  dis.DisambleChunk();
}
} // namespace qian

#endif  // QIAN_DEBUG_H_
