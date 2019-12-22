#include "debug.h"
#include "inst.h"

namespace qian {

void Disambler::DisambleChunk() {
  printf("== %s ==\n", name_.c_str());
  for (int offset = 0; offset < chunk_->Size(); ) {
    printf("%04d ", offset);
    Inst* inst = DispathInst(chunk_, offset);
    inst->DebugInfo();
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
