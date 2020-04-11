#include "xyxy/vm.h"

#include <glog/logging.h>

#include "xyxy/type.h"

namespace xyxy {

std::string Inst::DebugInfo() {
  std::string ret;
  ret += name_;
  ret += string(kDumpWidth - name_.size(), ' ');
  for (int i = 0; i < operands_.size(); i++) {
    Value val = operands_[i];
    ret += val.ToString();
  }
  ret += "\n";
  return ret;
}

#define DEFINE_INST(opcode, length) DEFINE_INST_HELPER(#opcode, opcode, length)

#define DEFINE_INST_HELPER(name, opcode, length) \
  class Inst_##opcode : public Inst {            \
   public:                                       \
    Inst_##opcode() {                            \
      this->opcode_ = opcode;                    \
      this->name_ = name;                        \
      this->length_ = length;                    \
    }                                            \
  };

DEFINE_INST(OP_RETURN, 1)
DEFINE_INST(OP_CONSTANT, 2)
DEFINE_INST(OP_NEGATE, 1)
DEFINE_INST(OP_ADD, 1)
DEFINE_INST(OP_SUB, 1)
DEFINE_INST(OP_MUL, 1)
DEFINE_INST(OP_DIV, 1)
DEFINE_INST(OP_NIL, 1)
DEFINE_INST(OP_TRUE, 1)
DEFINE_INST(OP_FALSE, 1)
DEFINE_INST(OP_NOT, 1)
DEFINE_INST(OP_EQUAL, 1)
DEFINE_INST(OP_GREATER, 1)
DEFINE_INST(OP_LESS, 1)
DEFINE_INST(OP_PRINT, 1)
DEFINE_INST(OP_POP, 1)
DEFINE_INST(OP_DEFINE_GLOBAL, 2)
DEFINE_INST(OP_SET_GLOBAL, 2)
DEFINE_INST(OP_GET_GLOBAL, 2)

VM::VM(Chunk* chunk) : chunk_(chunk) { pc_ = 0; }

#define CREATE_INST_INSTANCE(inst)          \
  case inst: {                              \
    return std::make_unique<Inst_##inst>(); \
  }

static std::unique_ptr<Inst> DispatchInst(uint8 opcode) {
  switch (opcode) {
    CREATE_INST_INSTANCE(OP_RETURN)
    CREATE_INST_INSTANCE(OP_CONSTANT)
    CREATE_INST_INSTANCE(OP_NEGATE)
    CREATE_INST_INSTANCE(OP_ADD)
    CREATE_INST_INSTANCE(OP_SUB)
    CREATE_INST_INSTANCE(OP_MUL)
    CREATE_INST_INSTANCE(OP_DIV)
    CREATE_INST_INSTANCE(OP_NIL)
    CREATE_INST_INSTANCE(OP_TRUE)
    CREATE_INST_INSTANCE(OP_FALSE)
    CREATE_INST_INSTANCE(OP_NOT)
    CREATE_INST_INSTANCE(OP_EQUAL)
    CREATE_INST_INSTANCE(OP_GREATER)
    CREATE_INST_INSTANCE(OP_LESS)
    CREATE_INST_INSTANCE(OP_PRINT)
    CREATE_INST_INSTANCE(OP_POP)
    CREATE_INST_INSTANCE(OP_DEFINE_GLOBAL)
    CREATE_INST_INSTANCE(OP_SET_GLOBAL)
    CREATE_INST_INSTANCE(OP_GET_GLOBAL)
    default: {
      CHECK(false);
      break;
    }
  }
  return nullptr;
}

std::unique_ptr<Inst> VM::CreateInst(uint8 offset) {
  OpCode byte = (OpCode)chunk_->GetByte(offset);
  auto inst = DispatchInst(byte);
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

std::string VM::DumpInsts() {
  std::string ret;
  for (int pc = 0; pc < chunk_->Size(); ) {
    auto inst = CreateInst(pc);
    ret += inst->DebugInfo();
    pc += inst->Length();
  }
  return ret;
}

Status VM::Run() {
  for (; pc_ < chunk_->Size();) {
    auto inst = CreateInst(pc_);
    std::cout << inst->DebugInfo();
    switch (inst->opcode_) {
      case OP_RETURN: {
        break;
      }
      case OP_CONSTANT: {
        CHECK(!inst->operands_.empty());
        stack_.Push(inst->operands_[0]);
        break;
      }
      case OP_NEGATE: {
        if (!stack_.Top().IsFloat()) {
          return Status(RUNTIME_ERROR, "Operand must be a number.");
        }
        Value val = stack_.Pop();
        stack_.Push(Value(-val.AsFloat()));
        break;
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
        break;
      }
      case OP_SUB: {
        BINARY_OP(-);
        break;
      }
      case OP_MUL: {
        BINARY_OP(*);
        break;
      }
      case OP_DIV: {
        BINARY_OP(/);
        break;
      }
      case OP_NIL: {
        stack_.Push(XYXY_NIL);
        break;
      }
      case OP_TRUE: {
        stack_.Push(Value(true));
        break;
      }
      case OP_FALSE: {
        stack_.Push(Value(false));
        break;
      }
      case OP_NOT: {
        stack_.Push(Value(stack_.Pop().IsFalsey()));
        break;
      }
      case OP_EQUAL: {
        BINARY_OP(==);
        break;
      }
      case OP_GREATER: {
        BINARY_OP(>);
        break;
      }
      case OP_LESS: {
        BINARY_OP(<);
        break;
      }
      case OP_PRINT: {
        printf("%s\n", stack_.Pop().ToString().c_str());
        break;
      }
      case OP_POP: {
        stack_.Pop();
        break;
      }
      case OP_DEFINE_GLOBAL: {
        CHECK(!inst->operands_.empty());
        Value val = inst->operands_[0];
        global_.Insert(val.ToString(), val);
        stack_.Pop();
        break;
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
        break;
      }
      default: {
        CHECK(false);
        break;
      }
    }
    pc_ += inst->Length();
  }
  return Status();
}

}  // namespace xyxy
