#include "vm.h"

#include "type.h"

namespace qian {

Status VM::Run() {
  for (; pc_ < chunk_->Size();) {
    Inst* inst = DispathInst(chunk_, pc_);
#ifndef NDEBUG
    inst->RunDebugInfo();
#endif
    Status status = Run(inst);
    pc_ += inst->Length();
    // TODO(zq7):
    // 1) Handle errors according to different status.
    //    Add something like this:
    //      error_->HandleError(status);
    // 2) Consider replact Inst* to Inst.
    delete inst;
  }
  return Status();
}

Status VM::Run(Inst* inst) {
  auto f = GlobalFunc()->Get(inst->Opcode());
  return f(this);
}

REGISTER_FUNC().Name("OP_RETURN").Func([](VM* vm) {
  DumpStack(vm->GetStack());
  return Status();
});

REGISTER_FUNC().Name("OP_CONSTANT").Func([](VM* vm) {
  // Get the current running inst.
  Inst* inst = DispathInst(vm->GetChunk(), vm->PC());
  auto oprds = inst->Operands();
  vm->GetStack()->Push(oprds->Get(0));
  return Status();
});

REGISTER_FUNC().Name("OP_NEGATE").Func([](VM* vm) {
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

REGISTER_FUNC().Name("OP_ADD").Func([](VM* vm) { BINARY_OP(QIAN_NUMBER, +); });

REGISTER_FUNC().Name("OP_SUB").Func([](VM* vm) { BINARY_OP(QIAN_NUMBER, -); });

REGISTER_FUNC().Name("OP_MUL").Func([](VM* vm) { BINARY_OP(QIAN_NUMBER, *); });

REGISTER_FUNC().Name("OP_DIV").Func([](VM* vm) { BINARY_OP(QIAN_NUMBER, /); });

REGISTER_FUNC().Name("OP_NIL").Func([](VM* vm) {
  vm->GetStack()->Push(QIAN_NIL);
  return Status();
});

REGISTER_FUNC().Name("OP_TRUE").Func([](VM* vm) {
  vm->GetStack()->Push(QIAN_BOOL(true));
  return Status();
});

REGISTER_FUNC().Name("OP_FALSE").Func([](VM* vm) {
  vm->GetStack()->Push(QIAN_BOOL(false));
  return Status();
});

REGISTER_FUNC().Name("OP_NOT").Func([](VM* vm) {
  auto stk = vm->GetStack();
  stk->Push(QIAN_BOOL(is_falsey(stk->Pop())));
  return Status();
});

REGISTER_FUNC().Name("OP_EQUAL").Func([](VM* vm) {
  auto stk = vm->GetStack();
  Value a = stk->Pop();
  Value b = stk->Pop();
  stk->Push(QIAN_BOOL(a == b));
  return Status();
});

REGISTER_FUNC().Name("OP_GREATER").Func([](VM* vm) {
  BINARY_OP(QIAN_BOOL, >);
});

REGISTER_FUNC().Name("OP_LESS").Func([](VM* vm) { BINARY_OP(QIAN_BOOL, <); });

}  // namespace qian
