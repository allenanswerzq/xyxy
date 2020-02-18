#include "qian/inst.h"

#include "qian/inst_impl.h"

namespace qian {

Inst* CreateInst(OpCode byte) {
  switch (byte) {
    case OP_RETURN:
      return new Inst_OP_RETURN();
    case OP_CONSTANT:
      return new Inst_OP_CONSTANT();
    case OP_NEGATE:
      return new Inst_OP_NEGATE();
    case OP_ADD:
      return new Inst_OP_ADD();
    case OP_SUB:
      return new Inst_OP_SUB();
    case OP_MUL:
      return new Inst_OP_MUL();
    case OP_DIV:
      return new Inst_OP_DIV();
    case OP_NIL:
      return new Inst_OP_NIL();
    case OP_TRUE:
      return new Inst_OP_TRUE();
    case OP_FALSE:
      return new Inst_OP_FALSE();
    case OP_NOT:
      return new Inst_OP_NOT();
    case OP_EQUAL:
      return new Inst_OP_EQUAL();
    case OP_GREATER:
      return new Inst_OP_GREATER();
    case OP_LESS:
      return new Inst_OP_LESS();
    default:
      break;
  }
  return nullptr;
}

Inst* DispathInst(Chunk* chunk, uint8 offset) {
  OpCode byte = (OpCode)chunk->GetByte(offset);
  Inst* inst = CreateInst(byte);
  for (int i = 1; i <= inst->Length() - 1; i++) {
    // Get the index of the value.
    int index = chunk->GetByte(offset + i);
    // NOTE: get the value out from chunk.
    inst->Operand(chunk->GetValue(index));
  }
  return inst;
}

}  // namespace qian
