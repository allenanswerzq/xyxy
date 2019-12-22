#ifndef QIAN_DEBUG_H_
#define QIAN_DEBUG_H_

#include "inst.h"
#include "chunk.h"

namespace qian {

class Disambler {
 public:
  Disambler(Chunk* chunk, const string& name)
    : chunk_(chunk), name_(name) {}

  virtual ~Disambler() {}

  void DisambleChunk();

  void DisambleInst(Inst* inst);

 private:
  Chunk* chunk_; // not owned
  string name_;
};

void DisambleChunk(Chunk* chunk, const string& name);

} // namespace qian

#endif  // QIAN_DEBUG_H_
