#ifndef QIAN_INST_H_
#define QIAN_INST_H_

#include "vector.h"
#include "chunk.h"

namespace qian {
class Inst {
 public:
  Inst() {}
  virtual ~Inst() {}

  typedef std::function<void(Inst*)> FuncDebugInfo;

  void Name(const string& name) { name_ = name; }
  string Name() { return name_; }

  void Length(uint8_t length) { length_ = length; }
  uint8_t Length() { return length_; }

  void DebugInfo(const FuncDebugInfo& f) { debug_info_ = f; }
  void DebugInfo() { debug_info_(this); }

  void Opcode(uint8_t opcode) { opcode_ = opcode; }
  void Operand(uint8_t operand) { operands_.Write(operand); }
  Vector<uint8_t>* Operands() { return &operands_; }

 private:
  string name_;
  uint8_t opcode_;
  uint8_t length_;
  Vector<uint8_t> operands_;
  FuncDebugInfo debug_info_;
};

Vector<Inst*>* Global() {
  static Vector<Inst*>* registry_;
  if (registry_ == nullptr) {
    registry_ = new Vector<Inst*> ();
  }
  return registry_;
}

Inst* GetInst(Chunk* chunk, uint8_t offset) {
  uint8_t byte = chunk->GetCode(offset);
  Inst* inst = Global()->Get(byte);
  for (int i = 0; i < inst->Length(); i++) {
    // Get the index of the value.
    int index = chunk->GetCode(offset + i);
    inst->Operand(index);
    // TODO(zq7): Get the real value from chunk.
    // inst->Value(chunk->GetValue(index));
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
  .Opcode(0)
  .Length(1)
  .DebugInfo([](Inst* inst) {
    printf("%s\n", inst->Name().c_str());
  });

REGISTER_INST()
  .Name("OP_CONSTANT")
  .Opcode(1)
  .Length(2)
  .DebugInfo([](Inst* inst) {
    auto oprd = inst->Operands();
    uint32_t index = oprd->Get(0);
    printf("%-16s %4d '", inst->Name().c_str(), index);
  });

} // namespace register_inst

#endif  // QIAN_INST_H_
