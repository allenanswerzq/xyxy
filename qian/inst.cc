#include "inst.h"

namespace qian {

Inst* DispathInst(Chunk* chunk, uint8 offset) {
  uint8 byte = chunk->GetByte(offset);
  assert(byte < GlobalInst()->Size());
  Inst* inst = GlobalInst()->Get(byte);
  for (int i = 1; i <= inst->Length() - 1; i++) {
    // Get the index of the value.
    int index = chunk->GetByte(offset + i);
    // NOTE: get the value out from chunk.
    inst->Operand(chunk->GetValue(index));
  }
  return inst;
}

REGISTER_INST("OP_RETURN")
  .Opcode(OP_RETURN)
  .Length(1)
  .DebugInfo([](Inst* inst) {
    printf("%s\n", inst->Name().c_str());
  });

REGISTER_INST("OP_CONSTANT")
  .Opcode(OP_CONSTANT)
  .Length(2)
  .DebugInfo([](Inst* inst) {
    auto oprd = inst->Operands();
    Value val = oprd->Get(0);
    printf("%-16s %4f\n", inst->Name().c_str(), val);
  });

REGISTER_INST("OP_NEGATE")
  .Opcode(OP_NEGATE)
  .Length(2)
  .DebugInfo([](Inst* inst) {
  });

}  // namespace qian
