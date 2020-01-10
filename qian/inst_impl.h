#ifndef QIAN_INST_IMPL_H_
#define QIAN_INST_IMPL_H_

#include "chunk.h"
#include "inst.h"
#include "logging.h"
#include "status.h"

namespace qian {

#define DEFINE_INST(opcode, len, debug_info, run_func) \
  DEFINE_INST_HELPER(#opcode, opcode, len, debug_info, run_func)

#define DEFINE_INST_HELPER(name, opcode, len, debug_info, run_func) \
  class Inst_##opcode : public Inst {                               \
   public:                                                          \
    Inst_##opcode() {                                               \
      this->name_ = name;                                           \
      this->length_ = len;                                          \
      this->opcode_ = opcode;                                       \
      this->run_func_ = run_func;                                   \
      this->debug_info_ = debug_info;                               \
    };                                                              \
  };

DEFINE_INST(
    OP_RETURN, 1, [](Inst* inst) -> void { printf("OP_RETURN"); },
    [](VM* vm) -> Status {
      DumpStack(vm->GetStack());
      return Status();
    });

DEFINE_INST(
    OP_CONSTANT, 2,
    // debug_info
    [](Inst* inst) -> void {
      auto oprd = inst->Operands();
      printf("%-16s ", inst->Name().c_str());
      for (int i = 0; i < oprd->Size(); i++) {
        Value val = oprd->Get(i);
        printf("%0.4f ", AS_CXX_NUMBER(val));
      }
      printf("\n");
    },
    // run
    [](VM* vm) -> Status {
      auto inst = DispathInst(vm->GetChunk(), vm->PC());
      auto oprds = inst->Operands();
      vm->GetStack()->Push(oprds->Get(0));
      return Status();
    });

DEFINE_INST(
    OP_NEGATE, 1, [](Inst* inst) -> void { printf("OP_NEGATE"); },
    [](VM* vm) -> Status {
      auto stk = vm->GetStack();
      if (!IS_QIAN_NUMBER(stk->Top())) {
        return Status(RUNTIME_ERROR, "Operand must be a number.");
      }
      stk->Push(QIAN_NUMBER(-AS_CXX_NUMBER(stk->Pop())));
      return Status();
    });

#define BINARY_OP(qian_type, op)                                 \
  do {                                                           \
    auto stk = vm->GetStack();                                   \
    auto lhs = stk->Pop();                                       \
    auto rhs = stk->Pop();                                       \
    if (!IS_QIAN_NUMBER(lhs) || !IS_QIAN_NUMBER(rhs)) {          \
      return Status(RUNTIME_ERROR, "Operand must be a number."); \
    }                                                            \
    double a = AS_CXX_NUMBER(lhs);                               \
    double b = AS_CXX_NUMBER(rhs);                               \
    stk->Push(qian_type(a op b));                                \
    return Status();                                             \
  } while (false)

DEFINE_INST(
    OP_ADD, 1, [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status { BINARY_OP(QIAN_NUMBER, +); });

DEFINE_INST(
    OP_SUB, 1, [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status { BINARY_OP(QIAN_NUMBER, -); });

DEFINE_INST(
    OP_MUL, 1, [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status { BINARY_OP(QIAN_NUMBER, *); });

DEFINE_INST(
    OP_DIV, 1, [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status { BINARY_OP(QIAN_NUMBER, /); });

DEFINE_INST(
    OP_NIL, 1, [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status {
      vm->GetStack()->Push(QIAN_NIL);
      return Status();
    });

DEFINE_INST(
    OP_TRUE, 1,
    [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status {
      vm->GetStack()->Push(QIAN_BOOL(true));
      return Status();
    });

DEFINE_INST(
    OP_FALSE, 1,
    [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status {
      vm->GetStack()->Push(QIAN_BOOL(false));
      return Status();
    });

DEFINE_INST(
    OP_NOT, 1, [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status {
      auto stk = vm->GetStack();
      stk->Push(QIAN_BOOL(is_falsey(stk->Pop())));
      return Status();
    });

DEFINE_INST(
    OP_EQUAL, 1,
    [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status {
      auto stk = vm->GetStack();
      Value a = stk->Pop();
      Value b = stk->Pop();
      stk->Push(QIAN_BOOL(a == b));
      return Status();
    });

DEFINE_INST(
    OP_GREATER, 1,
    [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status { BINARY_OP(QIAN_BOOL, >); });

DEFINE_INST(
    OP_LESS, 1,
    [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); },
    [](VM* vm) -> Status { BINARY_OP(QIAN_BOOL, <); });

}  // namespace qian

#endif  // QIAN_INST_IMPL_H_
