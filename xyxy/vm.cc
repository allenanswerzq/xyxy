#include "xyxy/vm.h"

#include "xyxy/logging.h"
#include "xyxy/type.h"

namespace xyxy {

#define OPCODE_NAME(opcode) #opcode

#define SIMPLE_INST(opcode)                             \
  case opcode: {                                        \
    std::string name = OPCODE_NAME(opcode);             \
    dis += name;                                        \
    dis += std::string(kDisassWidth - dis.size(), ' '); \
    LOGvv << dis;                                       \
    if (opcode == OP_RETURN) return;                    \
    break;                                              \
  }

#define CONSTANT_INST(opcode)                           \
  case opcode: {                                        \
    std::string name = OPCODE_NAME(opcode);             \
    dis += name;                                        \
    dis += std::string(kDisassWidth - dis.size(), ' '); \
    dis += ReadConstant().ToString();                   \
    LOGvv << dis;                                       \
    break;                                              \
  }

#define JUMP_INST(opcode) SIMPLE_INST(opcode)
#define BYTE_INST(opcode) SIMPLE_INST(opcode)

void VM::DisassFrame(CallFrame frame) {
  LOGvv << "-------------Raw Code----------------";
  CallFrame curr = curr_frame_;
  for (; curr_frame_.ip < GetChunk()->size(); ) {
    uint8 inst = ReadByte();
    string dis;
    char buf[64];
    snprintf(buf, 64, "%010d ", curr_frame_.ip);
    dis += string(buf);
    switch (inst) {
      CONSTANT_INST(OP_CONSTANT)
      CONSTANT_INST(OP_GET_GLOBAL)
      CONSTANT_INST(OP_DEFINE_GLOBAL)
      CONSTANT_INST(OP_SET_GLOBAL)
      SIMPLE_INST(OP_NIL)
      SIMPLE_INST(OP_TRUE)
      SIMPLE_INST(OP_FALSE)
      SIMPLE_INST(OP_POP)
      BYTE_INST(OP_GET_LOCAL)
      BYTE_INST(OP_SET_LOCAL)
      SIMPLE_INST(OP_EQUAL)
      SIMPLE_INST(OP_GREATER)
      SIMPLE_INST(OP_LESS)
      SIMPLE_INST(OP_ADD)
      SIMPLE_INST(OP_SUB)
      SIMPLE_INST(OP_MUL)
      SIMPLE_INST(OP_DIV)
      SIMPLE_INST(OP_NOT)
      SIMPLE_INST(OP_NEGATE)
      SIMPLE_INST(OP_PRINT)
      JUMP_INST(OP_JUMP)
      JUMP_INST(OP_JUMP_IF_FALSE)
      JUMP_INST(OP_LOOP)
      SIMPLE_INST(OP_RETURN)
      default: {
        CHECK(false) << "Unknown opcode: " << inst;
        break;
      }
    }
  }
  curr_frame_ = curr;
  LOGvv << "---------------------------------------";
}

void VM::DumpStack() {
  LOGvv << "-------------DUMP STACK----------------";
  for (int i = 0; i < stack_.Size(); i++) {
    LOGvv << stack_.Get(i).ToString();
  }
  LOGvv << "---------------------------------------";
}

VM::VM(ObjFunction* function) {
  stack_.Push(Value(function));
  CallFrame frame;
  frame.function = function;
  frame.ip = 0;
  frame.offset = 1;
  frames_.push_back(frame);
  curr_frame_ = frame;
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

Status VM::Call(Value* callee, int arg_count) {
  CHECK(callee->IsObject());
  if (callee->IsFunction()) {
    return Call(callee->AsFunction(), arg_count);
  }
  return Status();
}

Status VM::Call(ObjFunction* func, int arg_count) {
  if (arg_count != func->ArgNum()) {
    CHECK(false);
  }
  if (frames_.size() == FRAME_MAX) {
    CHECK(false);
  }
  CallFrame frame;
  frame.function = func;
  frame.ip = 0;
  frame.offset = stack_.Size() - arg_count - 1;
  frames_.push_back(frame);
  curr_frame_ = frames_.back();
  return Status();
}

Status VM::Run() {
  // DisassFrame(curr_frame_);
  for (;;) {
    uint8 inst = ReadByte();
    switch (inst) {
      case OP_CONSTANT: {
        Value val = ReadConstant();
        LOGcc << "Define constant: " << val.ToString();
        stack_.Push(val);
        break;
      }
      case OP_NIL: {
        stack_.Push(Value());
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
      case OP_POP: {
        LOGcc << "Pop out: " << stack_.Top().ToString();
        stack_.Pop();
        break;
      }
      case OP_GET_LOCAL: {
        uint8 slot = ReadByte();
        auto val = GetFrame(slot);
        LOGcc << "Get local: " << val.ToString();
        stack_.Push(val);
        break;
      }
      case OP_SET_LOCAL: {
        uint8 slot = ReadByte();
        auto val = stack_.Top();

        LOGcc << "Set local: " << val.ToString();
        SetFrame(slot, val);
        break;
      }
      case OP_GET_GLOBAL: {
        string name = ReadConstant().AsString();
        Value val;
        if (!global_.Find(name, &val)) {
          CHECK(false);
        }
        LOGcc << "Get global: " << name << " " << val.ToString();
        stack_.Push(val);
        break;
      }
      case OP_DEFINE_GLOBAL: {
        string name = ReadConstant().AsString();
        LOGcc << "Define global: " << name << " " << stack_.Top().ToString();
        global_.Insert(name, stack_.Pop());
        break;
      }
      case OP_SET_GLOBAL: {
        string name = ReadConstant().AsString();
        if (!global_.Find(name)) {
          CHECK(false);
        }

        LOGcc << "Set global: " << name << " " << stack_.Top().ToString();

        global_.Insert(name, stack_.Top());
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
          LOGcc << "Binary add: " << lhs.ToString() << " " << rhs.ToString();
          if (!rhs.IsString()) {
            return Status(RUNTIME_ERROR, "Operand must be a string.");
          }
          string a = lhs.AsString();
          string b = rhs.AsString();

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
        auto val = stack_.Pop();
        final_print_ = val.ToString();
        printf("%s\n", val.ToString().c_str());
        break;
      }
      case OP_JUMP: {
        uint16 offset = ReadShort();
        curr_frame_.ip += offset;
        LOGcc << "Jump over " << offset << " to " << curr_frame_.ip;
      }
      case OP_JUMP_IF_FALSE: {
        uint16 offset = ReadShort();
        if (stack_.Top().IsFalsey()) {
          curr_frame_.ip += offset;
          LOGcc << "Jump over " << offset << " to " << curr_frame_.ip;
        }
        break;
      }
      case OP_LOOP: {
        uint16 offset = ReadShort();
        curr_frame_.ip -= offset;
        LOGcc << "Jump back " << offset << " to " << curr_frame_.ip;
        continue;
      }
      case OP_CALL: {
        int arg_count = ReadByte();
        auto val = GetFrame(arg_count);
        if (Call(&val, arg_count) != Status()) {
          CHECK(false);
        }
        curr_frame_ = frames_.back();
        break;
      }
      case OP_RETURN: {
        LOGcc << "Returning from " << curr_frame_.function->ToString();
        auto ret = stack_.Pop();
        frames_.pop_back();
        if (frames_.empty()) {
          stack_.Pop();
          return Status();
        }

        stack_.Shrink(curr_frame_.offset);
        stack_.Push(ret);
        curr_frame_ = frames_.back();
        if (!stack_.Empty()) {
          DumpStack();
        }
        break;
      }
      default: {
        CHECK(false) << "Unkown inst to run.";
        break;
      }
    }
  }
  return Status();
}
}  // namespace xyxy
