#include "xyxy/vm.h"

#include "xyxy/logging.h"
#include "xyxy/type.h"

namespace xyxy {

const int Inst::kDumpWidth = 20;

void Inst::DebugInfo() {
  std::string ret;
  char buf[64];
  snprintf(buf, 64, "%010d", address_);
  ret += string(buf);
  ret += " ";
  ret += name_;
  ret += string(kDumpWidth - name_.size(), ' ');
  for (size_t i = 0; i < operands_.size(); i++) {
    Value val = operands_[i];
    ret += val.ToString();
  }
  for (size_t i = 0; i < metadata_.size(); i++) {
    if (i > 0) {
      ret += " ";
    }
    ret += std::to_string(metadata_[i]);
  }
  LOGvv << ret;
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
DEFINE_INST(OP_SET_LOCAL, 2)
DEFINE_INST(OP_GET_LOCAL, 2)
DEFINE_INST(OP_JUMP_IF_FALSE, 3)
DEFINE_INST(OP_JUMP, 3)
DEFINE_INST(OP_LOOP, 3)

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
    CREATE_INST_INSTANCE(OP_SET_LOCAL)
    CREATE_INST_INSTANCE(OP_GET_LOCAL)
    CREATE_INST_INSTANCE(OP_JUMP_IF_FALSE)
    CREATE_INST_INSTANCE(OP_JUMP)
    CREATE_INST_INSTANCE(OP_LOOP)
    default: {
      CHECK(false);
      break;
    }
  }
  return nullptr;
}

// TODO(): refact this function.
std::unique_ptr<Inst> VM::CreateInst(uint8 offset) {
  OpCode byte = (OpCode)chunk_->GetByte(offset);
  auto inst = DispatchInst(byte);
  inst->address_ = offset;
  if (byte == OP_SET_LOCAL || byte == OP_GET_LOCAL) {
    CHECK(inst->metadata_.empty());
    inst->metadata_.push_back(chunk_->GetByte(offset + 1));
  }
  else if (byte == OP_JUMP_IF_FALSE || byte == OP_JUMP || byte == OP_LOOP) {
    CHECK(inst->metadata_.empty());
    inst->metadata_.push_back(chunk_->GetByte(offset + 1));
    inst->metadata_.push_back(chunk_->GetByte(offset + 2));
  }
  else {
    for (int i = 1; i <= inst->Length() - 1; i++) {
      int index = chunk_->GetByte(offset + i);
      Value val = chunk_->GetConstant(index);
      inst->AddOperand(val);
    }
  }
  return inst;
}

#define BINARY_OP(op)                                                \
  do {                                                               \
    auto rhs = stack_.Pop();                                         \
    auto lhs = stack_.Pop();                                         \
    if (lhs.IsFloat()) {                                             \
      if (!rhs.IsFloat()) {                                          \
        return Status(RUNTIME_ERROR, "Operand must be a number.");   \
      }                                                              \
      double a = lhs.AsFloat();                                      \
      double b = rhs.AsFloat();                                      \
      Value res = Value(a op b);                                     \
      LOGcc << "Binary op: " << lhs.ToString() << " " << #op << " "  \
            << rhs.ToString() << " = " << res.ToString();            \
      stack_.Push(res);                                              \
      break;                                                         \
    }                                                                \
    else {                                                           \
      return Status(RUNTIME_ERROR, "Unsupported binary operation."); \
    }                                                                \
  } while (false)

void VM::DumpInsts() {
  LOGvv << "-------------RAW CODE-------------------";
  for (int pc = 0; pc < chunk_->size();) {
    auto inst = CreateInst(pc);
    inst->DebugInfo();
    pc += inst->Length();
  }
  LOGvv << "---------------------------------------";
}

Status VM::Run() {
  DumpInsts();
  for (; pc_ < chunk_->size();) {
    auto inst = CreateInst(pc_);
    inst->DebugInfo();
    switch (inst->opcode_) {
      case OP_RETURN: {
        break;
      }
      case OP_CONSTANT: {
        CHECK(!inst->operands_.empty());
        LOGcc << "Define constant: " << inst->operands_[0].ToString();
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
        // TODO(): support a += b.
        if (stack_.Top().IsString()) {
          auto lhs = stack_.Pop();
          auto rhs = stack_.Pop();
          LOGcc << "Binary add: " << lhs.ToString() << " " << rhs.ToString();
          if (!rhs.IsString()) {
            return Status(RUNTIME_ERROR, "Operand must be a string.");
          }
          string a = lhs.AsString();
          string b = rhs.AsString();
          // NOTE: the order here must be `b + a`.
          b += a;
          stack_.Push(Value(new ObjString(b)));
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
        final_print_ = stack_.Top().ToString();
        printf("%s\n", stack_.Pop().ToString().c_str());
        LOGcc << "Stack size after print: " << stack_.Size();
        break;
      }
      case OP_POP: {
        stack_.Pop();
        break;
      }
      case OP_DEFINE_GLOBAL: {
        // Pop one value out from stack and assign it as the global variable.
        CHECK(!inst->operands_.empty());
        std::string var_name = inst->operands_[0].ToString();
        LOGcc << "Define global: " << var_name << " "
              << stack_.Top().ToString();
        global_.Insert(var_name, stack_.Pop());
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
        LOGcc << "Get global: " << var_name << " " << val.ToString();
        stack_.Push(val);
        break;
      }
      case OP_SET_GLOBAL: {
        CHECK(!inst->operands_.empty());
        string var_name = inst->operands_[0].ToString();
        if (!global_.Find(var_name)) {
          // TODO(): Error handling
          CHECK(false);
        }
        // Sets to a new value.
        LOGcc << "Set global: " << var_name << " " << stack_.Top().ToString();
        // NOTE: here we dont pop the value from stack.
        global_.Insert(var_name, stack_.Top());
        break;
      }
      case OP_GET_LOCAL: {
        CHECK(!inst->metadata_.empty());
        uint8 slot = inst->metadata_[0];
        LOGcc << "Get local: " << stack_.Get(slot).ToString();
        stack_.Push(stack_.Get(slot));
        break;
      }
      case OP_SET_LOCAL: {
        CHECK(!inst->metadata_.empty());
        uint8 slot = inst->metadata_[0];
        // NOTE: here we dont pop the value from stack.
        LOGcc << "Set local: " << stack_.Top().ToString();
        stack_.Set(slot, stack_.Top());
        break;
      }
      case OP_JUMP_IF_FALSE: {
        auto meta = inst->metadata_;
        CHECK(meta.size() == 2);
        uint16_t count = (uint16_t)((meta[0] << 8) | meta[1]);
        // Also skip the inst itself.
        count += inst->Length();
        if (stack_.Top().IsFalsey()) {
          pc_ += count;
          LOGcc << "Jump over " << count << " to " << pc_;
          continue;
        }
        break;
      }
      case OP_JUMP: {
        auto meta = inst->metadata_;
        CHECK(meta.size() == 2);
        uint16_t count = (uint16_t)((meta[0] << 8) | meta[1]);
        count += inst->Length();
        pc_ += count;
        LOGcc << "Jump over " << count << " to " << pc_;
        continue;
      }
      case OP_LOOP: {
        auto meta = inst->metadata_;
        CHECK(meta.size() == 2);
        uint16_t count = (uint16_t)((meta[0] << 8) | meta[1]);
        pc_ -= count;
        LOGcc << "Jump back " << count << " to " << pc_;
        continue;
      }
      default: {
        CHECK(false) << "Unkown inst to run with pc: " << pc_;
        break;
      }
    }
    pc_ += inst->Length();
  }
  return Status();
}

}  // namespace xyxy
