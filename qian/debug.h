#ifndef QIAN_DEBUG_H_
#define QIAN_DEBUG_H_

#include "inst.h"
#include "chunk.h"

namespace qian {

class Disambler {
 public:
  Disambler(Chunk* chunk, const string& name);
  virtual ~Disambler() {}

  void DisambleChunk();

  void DisambleInst(qian::Inst* inst);

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
    printf("%04d ", offset);
    Inst* inst = DispathInst(chunk_, offset);
    DisambleInst(inst);
    offset += inst->Length();
  }
}

void Disambler::DisambleInst(Inst* inst) {
  inst->DebugInfo();
}

void DisambleChunk(Chunk* chunk, const string& name) {
  Disambler dis(chunk, name);
  dis.DisambleChunk();
}

} // namespace qian

#endif  // QIAN_DEBUG_H_
