#ifndef XYXY_INST_IMPL_H_
#define XYXY_INST_IMPL_H_

#include <glog/logging.h>

#include "xyxy/chunk.h"
#include "xyxy/inst.h"
#include "xyxy/status.h"

namespace xyxy {

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

#define DEFAULT_DEBUG_INFO \
  [](Inst* inst) -> void { printf("%s\n", inst->Name().c_str()); }

#define ARGS_DEBUG_INFO                         \
  [](Inst* inst) -> void {                      \
    std::vector<Value> oprd = inst->Operands(); \
    printf("%-16s ", inst->Name().c_str());     \
    for (int i = 0; i < oprd.size(); i++) {     \
      Value val = oprd[i];                      \
      printf("%s ", val.ToString().c_str());    \
    }                                           \
    printf("\n");                               \
  }

DEFINE_INST(OP_RETURN, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  // DumpStack(vm->GetStack());
  return Status();
});

DEFINE_INST(OP_CONSTANT, 2, ARGS_DEBUG_INFO,
            // --- run
            [](VM* vm) -> Status {
              auto inst = DispathInst(vm->GetChunk(), vm->PC());
              auto oprds = inst->Operands();
              vm->GetStack()->Push(oprds[0]);
              return Status();
            });

DEFINE_INST(OP_NEGATE, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  auto stk = vm->GetStack();
  if (!stk->Top().IsFloat()) {
    return Status(RUNTIME_ERROR, "Operand must be a number.");
  }
  Value val = stk->Pop();
  stk->Push(Value(-val.AsFloat()));
  return Status();
});

#define BINARY_OP(op)                                            \
  do {                                                           \
    auto stk = vm->GetStack();                                   \
    auto lhs = stk->Pop();                                       \
    auto rhs = stk->Pop();                                       \
    if (!lhs.IsFloat() || !rhs.IsFloat()) {                      \
      return Status(RUNTIME_ERROR, "Operand must be a number."); \
    }                                                            \
    double a = lhs.AsFloat();                                    \
    double b = rhs.AsFloat();                                    \
    stk->Push(Value(a op b));                                    \
    return Status();                                             \
  } while (false)

DEFINE_INST(OP_ADD, 1, DEFAULT_DEBUG_INFO,
            [](VM* vm) -> Status { BINARY_OP(+); });

DEFINE_INST(OP_SUB, 1, DEFAULT_DEBUG_INFO,
            [](VM* vm) -> Status { BINARY_OP(-); });

DEFINE_INST(OP_MUL, 1, DEFAULT_DEBUG_INFO,
            [](VM* vm) -> Status { BINARY_OP(*); });

DEFINE_INST(OP_DIV, 1, DEFAULT_DEBUG_INFO,
            [](VM* vm) -> Status { BINARY_OP(/); });

DEFINE_INST(OP_NIL, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  vm->GetStack()->Push(NILL);
  return Status();
});

DEFINE_INST(OP_TRUE, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  vm->GetStack()->Push(Value(true));
  return Status();
});

DEFINE_INST(OP_FALSE, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  vm->GetStack()->Push(Value(false));
  return Status();
});

DEFINE_INST(OP_NOT, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  auto stk = vm->GetStack();
  stk->Push(Value(stk->Pop().IsFalsey()));
  return Status();
});

DEFINE_INST(OP_EQUAL, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  auto stk = vm->GetStack();
  Value a = stk->Pop();
  Value b = stk->Pop();
  stk->Push(Value(a == b));
  return Status();
});

DEFINE_INST(OP_GREATER, 1, DEFAULT_DEBUG_INFO,
            [](VM* vm) -> Status { BINARY_OP(>); });

DEFINE_INST(OP_LESS, 1, DEFAULT_DEBUG_INFO,
            [](VM* vm) -> Status { BINARY_OP(<); });

DEFINE_INST(OP_PRINT, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  printf("%s\n", vm->GetStack()->Pop().ToString().c_str());
  return Status();
});

DEFINE_INST(OP_POP, 1, DEFAULT_DEBUG_INFO, [](VM* vm) -> Status {
  vm->GetStack()->Pop();
  return Status();
});

DEFINE_INST(OP_DEFINE_GLOBAL, 2, ARGS_DEBUG_INFO, [](VM* vm) -> Status {
  auto inst = DispathInst(vm->GetChunk(), vm->PC());
  assert(inst);
  assert(!inst->Operands().empty());
  Value val = inst->Operands()[0];
  assert(vm->GetGlobal());
  assert(vm->GetStack());
  vm->GetGlobal()->Insert(val.ToString(), val);
  vm->GetStack()->Pop();
  return Status();
});

DEFINE_INST(OP_GET_GLOBAL, 2, ARGS_DEBUG_INFO, [](VM* vm) -> Status {
  auto inst = DispathInst(vm->GetChunk(), vm->PC());
  assert(inst);
  assert(!inst->Operands().empty());
  std::string val_name = inst->Operands()[0].ToString();
  Value val;
  assert(vm->GetGlobal());
  assert(vm->GetStack());
  if (!vm->GetGlobal()->Find(val_name, &val)) {
    // TODO(): Error handling
    assert(false);
    return Status();
  }
  vm->GetStack()->Push(val);
  return Status();
});

}  // namespace xyxy

#endif  // XYXY_INST_IMPL_H_
