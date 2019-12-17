#ifndef QIAN_DEBUG_H_
#define QIAN_DEBUG_H_

#include "chunk.h"
#include "inst.h"

class Disassembler {
 public:
  Disassembler(Chunk* chunk, string& name);
  virtual ~Disassembler() {}

  void DisassembleChunk();

  int DisassembleInst(int offset);

 private:
  Chunk* chunk_;  // not owned
  string name_;
};

Disassembler::Disassembler(Chunk* chunk, string& name) {
  chunk_ = chunk;
  name_ = name;
}

void Disassembler::DisassembleChunk() {
  printf("== %s ==", name_.c_str());
  for (int offset = 0; offset < chunk_->Size();) {
    offset = DisassembleInst(offset);
  }
}

int Disassembler::DisassembleInst(int offset) {
  printf("%04d ", offset);
  uint8_t inst = chunk_->GetCode(offset);
  switch (inst) {
    case OP_RETURN: {
      printf("OP_RETURN\n");
      return offset + 1;
    }
    default: {
      printf("Unknow opcode %d\n", inst);
    }
  }
}

void DisassembleChunk(Chunk* chunk, string& name) {
  Disassembler dis(chunk, name);
  dis.DisassembleChunk();
}

#endif  // QIAN_DEBUG_H_
