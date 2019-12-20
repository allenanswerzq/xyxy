#ifndef QIAN_INST_H_
#define QIAN_INST_H_

#include "status.h"

namespace qian {

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
} OPCODE;

class VM;

class Inst {
 public:
  Inst() {}
  virtual ~Inst() {}

  typedef std::function<void(Inst*)> FuncDebugInfo;
  typedef std::function<Status(VM*, Inst*)> FuncRun;

  void Name(const string& name) { name_ = name; }
  string Name() { return name_; }

  void Length(uint8_t length) { length_ = length; }
  uint8_t Length() { return length_; }

  void DebugInfo(const FuncDebugInfo& f) { debug_info_ = f; }
  void DebugInfo() { debug_info_(this); }

  void Run(const FuncRun& f) { run_ = f; }
  Status Run(VM* vm) { return run_(vm, this); }

  void Opcode(uint8_t opcode) { opcode_ = opcode; }
  void Operand(Value operand) { operands_.Write(operand); }
  Vector<Value>* Operands() { return &operands_; }

 private:
  string name_;
  uint8_t opcode_;
  uint8_t length_;
  Vector<Value> operands_;
  FuncDebugInfo debug_info_;
  FuncRun run_;
};

Vector<Inst*>* Global() {
  static Vector<Inst*>* registry_;
  if (registry_ == nullptr) {
    registry_ = new Vector<Inst*> ();
  }
  return registry_;
}

Inst* DispathInst(Chunk* chunk, uint8_t offset) {
  uint8_t byte = chunk->GetByte(offset);
  Inst* inst = Global()->Get(byte);
  for (int i = 0; i < inst->Length(); i++) {
    // Get the index of the value.
    int index = chunk->GetByte(offset + i);
    // NOTE: get the value from chunk.
    inst->Operand(chunk->GetValue(index));
  }
  return inst;
}

} // namespace qian

namespace register_inst {
  using qian::Inst;
  struct InstDefWrapper {
    InstDefWrapper() {
      inst_ = new Inst();
      ::qian::Global()->Write(inst_);
    }
    InstDefWrapper& Name(const string& name) {
      inst_->Name(name);
      return *this;
    }
    InstDefWrapper& Length(uint8_t len) {
      inst_->Length(len);
      return *this;
    }
    InstDefWrapper& Opcode(uint8_t opcode) {
      inst_->Opcode(opcode);
      return *this;
    }
    InstDefWrapper& Operand(uint8_t operand) {
      inst_->Operand(operand);
      return *this;
    }
    InstDefWrapper& DebugInfo(const std::function<void(Inst*)>& f) {
      inst_->DebugInfo(f);
      return *this;
    }
    InstDefWrapper& Run(const std::function<Status(Inst*)>& f) {
      inst_->Run(f);
      return *this;
    }
   private:
    Inst* inst_;
  };

#define REGISTER_INST() REGISTER_INST_UNIQ_HELPER(__COUNTER__)
#define REGISTER_INST_UNIQ_HELPER(ctr) REGISTER_INST_UNIQ(ctr)
#define REGISTER_INST_UNIQ(ctr)                                 \
  static ::register_inst::InstDefWrapper register_inst##ctr     \
    __attribute__ ((unused)) = ::register_inst::InstDefWrapper()

//---------------------------------------------------------------------------

REGISTER_INST()
  .Name("OP_RETURN")
  .Opcode(OP_RETURN)
  .Length(1)
  .Run([](VM *vm, Inst* inst) {
    Value val = vm->Stack()->Pop();
    printf("%g\n", val);
    return Status();
  })
  .DebugInfo([](Inst* inst) {
    printf("%s\n", inst->Name().c_str());
  });


REGISTER_INST()
  .Name("OP_CONSTANT")
  .Opcode(OP_CONSTANT)
  .Length(2)
  .Run([](VM *vm, Inst* inst) {
    Value val = vm->GetChunk()->GetValue(0);
    vm->Stack()->Push(val);
    return Status();
  })
  .DebugInfo([](Inst* inst) {
    auto oprd = inst->Operands();
    uint32_t index = oprd->Get(0);
    printf("%-16s %4d '", inst->Name().c_str(), index);
  });

} // namespace register_inst

#endif  // QIAN_INST_H_
