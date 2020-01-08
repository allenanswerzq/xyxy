#include "inst.h"

namespace qian {

Inst* DispathInst(Chunk* chunk, uint8 offset) {
  OpCode byte = (OpCode)chunk->GetByte(offset);
  Inst* inst = Inst::Create(byte);
  // TODO(zq7): better handle inst operands, for now assume
  // all insts will have one `Value` as operand, but it maybe not
  // applicable in the future.
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
    .DebugInfo([](Inst* inst) { printf("%s\n", inst->Name().c_str()); });

REGISTER_INST("OP_CONSTANT")
    .Opcode(OP_CONSTANT)
    .Length(2)
    .DebugInfo([](Inst* inst) {
      auto oprd = inst->Operands();
      printf("%-16s ", inst->Name().c_str());
      for (int i = 0; i < oprd->Size(); i++) {
        Value val = oprd->Get(i);
        printf("%0.4f ", AS_CXX_NUMBER(val));
      }
      printf("\n");
    });

REGISTER_INST("OP_NEGATE")
    .Opcode(OP_NEGATE)
    .Length(1)
    .DebugInfo([](Inst* inst) {});

REGISTER_INST("OP_ADD").Opcode(OP_ADD).Length(1).DebugInfo([](Inst* inst) {
  printf("%s\n", inst->Name().c_str());
});

REGISTER_INST("OP_SUB").Opcode(OP_SUB).Length(1).DebugInfo([](Inst* inst) {
  printf("%s\n", inst->Name().c_str());
});

REGISTER_INST("OP_MUL").Opcode(OP_MUL).Length(1).DebugInfo([](Inst* inst) {
  printf("%s\n", inst->Name().c_str());
});

REGISTER_INST("OP_DIV").Opcode(OP_DIV).Length(1).DebugInfo([](Inst* inst) {
  printf("%s\n", inst->Name().c_str());
});

REGISTER_INST("OP_NIL").Opcode(OP_NIL).Length(1).DebugInfo([](Inst* inst) {
  printf("OP_NIL\n");
});

REGISTER_INST("OP_TRUE").Opcode(OP_TRUE).Length(1).DebugInfo([](Inst* inst) {
  printf("OP_TRUE\n");
});

REGISTER_INST("OP_FALSE").Opcode(OP_FALSE).Length(1).DebugInfo([](Inst* inst) {
  printf("OP_FALSE\n");
});

REGISTER_INST("OP_NOT").Opcode(OP_NOT).Length(1).DebugInfo([](Inst* inst) {
  printf("OP_NOT\n");
});

REGISTER_INST("OP_EQUAL").Opcode(OP_EQUAL).Length(1).DebugInfo([](Inst* inst) {
  printf("OP_EQUAL\n");
});

REGISTER_INST("OP_GREATER")
    .Opcode(OP_GREATER)
    .Length(1)
    .DebugInfo([](Inst* inst) { printf("OP_GREATER\n"); });

REGISTER_INST("OP_LESS").Opcode(OP_LESS).Length(1).DebugInfo([](Inst* inst) {
  printf("OP_LESS\n");
});

}  // namespace qian
