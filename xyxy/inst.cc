#include "xyxy/inst.h"

#include <glog/logging.h>

#include <memory>

#include "xyxy/inst_impl.h"

namespace xyxy {

static std::unique_ptr<Inst> create_inst(OpCode byte) {
  switch (byte) {
    case OP_RETURN:
      return std::make_unique<Inst_OP_RETURN>();
    case OP_CONSTANT:
      return std::make_unique<Inst_OP_CONSTANT>();
    case OP_NEGATE:
      return std::make_unique<Inst_OP_NEGATE>();
    case OP_ADD:
      return std::make_unique<Inst_OP_ADD>();
    case OP_SUB:
      return std::make_unique<Inst_OP_SUB>();
    case OP_MUL:
      return std::make_unique<Inst_OP_MUL>();
    case OP_DIV:
      return std::make_unique<Inst_OP_DIV>();
    case OP_NIL:
      return std::make_unique<Inst_OP_NIL>();
    case OP_TRUE:
      return std::make_unique<Inst_OP_TRUE>();
    case OP_FALSE:
      return std::make_unique<Inst_OP_FALSE>();
    case OP_NOT:
      return std::make_unique<Inst_OP_NOT>();
    case OP_EQUAL:
      return std::make_unique<Inst_OP_EQUAL>();
    case OP_GREATER:
      return std::make_unique<Inst_OP_GREATER>();
    case OP_LESS:
      return std::make_unique<Inst_OP_LESS>();
    case OP_PRINT:
      return std::make_unique<Inst_OP_PRINT>();
    case OP_POP:
      return std::make_unique<Inst_OP_POP>();
    case OP_DEFINE_GLOBAL:
      return std::make_unique<Inst_OP_DEFINE_GLOBAL>();
    case OP_GET_GLOBAL:
      return std::make_unique<Inst_OP_GET_GLOBAL>();
    default:
      break;
  }
  return nullptr;
}

std::unique_ptr<Inst> DispathInst(std::shared_ptr<Chunk> chunk, uint8 offset) {
  OpCode byte = (OpCode)chunk->GetByte(offset);
  auto inst = create_inst(byte);
  VLOG(1) << "DispathInst: OFF:" << std::to_string(offset) << " BYTE: " << byte
          << " " << inst->Name();
  for (int i = 1; i <= inst->Length() - 1; i++) {
    // Get the index of the constants assoicated with this inst.
    int index = chunk->GetByte(offset + i);
    // Get the value out from chunk.
    Value val = chunk->GetConstant(index);
    // Put it as the operand of this inst.
    inst->AddOperand(val);
  }
  return inst;
}

}  // namespace xyxy
