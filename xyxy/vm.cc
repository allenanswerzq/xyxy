#include "xyxy/vm.h"

#include <glog/logging.h>

#include "xyxy/type.h"

namespace xyxy {

void Inst::DebugInfo() {
  printf("%-16s ", name_.c_str());
  for (int i = 0; i < operands_.size(); i++) {
    Value val = operands_[i];
    printf("%s ", val.ToString().c_str());
  }
  printf("\n");
}

VM::VM(std::shared_ptr<Chunk> chunk) : chunk_(chunk) { pc_ = 0; }

std::unique_ptr<Inst> VM::CreateInst(uint8 offset) {
  OpCode byte = (OpCode)chunk_->GetByte(offset);
  auto inst = std::make_unique<Inst>();
  inst->opcode_ = byte;
  for (int i = 1; i <= inst->Length() - 1; i++) {
    int index = chunk_->GetByte(offset + i);
    Value val = chunk_->GetConstant(index);
    inst->AddOperand(val);
  }
  return inst;
}

#define BINARY_OP(op)                                                \
  do {                                                               \
    auto lhs = stack_.Pop();                                         \
    auto rhs = stack_.Pop();                                         \
    if (lhs.IsFloat()) {                                             \
      if (!rhs.IsFloat()) {                                          \
        return Status(RUNTIME_ERROR, "Operand must be a number.");   \
      }                                                              \
      double a = lhs.AsFloat();                                      \
      double b = rhs.AsFloat();                                      \
      stack_.Push(Value(a op b));                                    \
      return Status();                                               \
    }                                                                \
    else {                                                           \
      return Status(RUNTIME_ERROR, "Unsupported binary operation."); \
    }                                                                \
  } while (false)

Status VM::Run() {
  for (; pc_ < chunk_->Size();) {
    auto inst = CreateInst(pc_);
    inst->DebugInfo();
    switch (inst->opcode_) {
      case OP_RETURN: {
        return Status();
      }
      case OP_CONSTANT: {
        CHECK(!inst->operands_.empty());
        stack_.Push(inst->operands_[0]);
        return Status();
      }
      case OP_NEGATE: {
        if (!stack_.Top().IsFloat()) {
          return Status(RUNTIME_ERROR, "Operand must be a number.");
        }
        Value val = stack_.Pop();
        stack_.Push(Value(-val.AsFloat()));
        return Status();
      }
      case OP_ADD: {
        if (stack_.Top().IsString()) {
          auto lhs = stack_.Pop();
          auto rhs = stack_.Pop();
          if (!rhs.IsString()) {
            return Status(RUNTIME_ERROR, "Operand must be a string.");
          }
          string a = lhs.AsString();
          string b = rhs.AsString();
          a += b;
          stack_.Push(Value(new ObjString(a)));
        }
        else {
          BINARY_OP(+);
        }
        return Status();
      }
      case OP_SUB: {
        BINARY_OP(-);
        return Status();
      }
      case OP_MUL: {
        BINARY_OP(*);
        return Status();
      }
      case OP_DIV: {
        BINARY_OP(/);
        return Status();
      }
      case OP_NIL: {
        stack_.Push(XYXY_NIL);
        return Status();
      }
      case OP_TRUE: {
        stack_.Push(Value(true));
        return Status();
      }
      case OP_FALSE: {
        stack_.Push(Value(false));
        return Status();
      }
      case OP_NOT: {
        stack_.Push(Value(stack_.Pop().IsFalsey()));
        return Status();
      }
      case OP_EQUAL: {
        BINARY_OP(==);
        return Status();
      }
      case OP_GREATER: {
        BINARY_OP(>);
        return Status();
      }
      case OP_LESS: {
        BINARY_OP(<);
        return Status();
      }
      case OP_PRINT: {
        printf("%s\n", stack_.Pop().ToString().c_str());
        return Status();
      }
      case OP_POP: {
        stack_.Pop();
        return Status();
      }
      case OP_DEFINE_GLOBAL: {
        CHECK(!inst->operands_.empty());
        Value val = inst->operands_[0];
        global_.Insert(val.ToString(), val);
        stack_.Pop();
        return Status();
      }
      case OP_GET_GLOBAL: {
        CHECK(!inst->operands_.empty());
        string var_name = inst->operands_[0].ToString();
        Value val;
        if (!global_.Find(var_name, &val)) {
          // TODO(): Error handling
          CHECK(false);
        }
        stack_.Push(val);
        return Status();
      }
      default: {
        CHECK(false);
        break;
      }
    }
  }
  return Status();
}

}  // namespace xyxy
