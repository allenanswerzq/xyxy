#include "inst.h"

namespace qian {

Inst* DispathInst(Chunk* chunk, uint8_t offset) {
  uint8_t byte = chunk->GetByte(offset);
  Inst* inst = Global()->Get(byte);
  for (int i = 0; i < inst->Length() - 1; i++) {
    // Get the index of the value.
    int index = chunk->GetByte(offset + i);
    // NOTE: get the value out from chunk.
    inst->Operand(chunk->GetValue(index));
  }
  return inst;
}

REGISTER_INST()
  .Name("OP_RETURN")
  .Opcode(OP_RETURN)
  .Length(1)
  .DebugInfo([](Inst* inst) {
    printf("%s\n", inst->Name().c_str());
  });

REGISTER_INST()
  .Name("OP_CONSTANT")
  .Opcode(OP_CONSTANT)
  .Length(2)
  .DebugInfo([](Inst* inst) {
    auto oprd = inst->Operands();
    uint32_t index = oprd->Get(0);
    printf("%-16s %4d '", inst->Name().c_str(), index);
  });

}  // namespace qian
