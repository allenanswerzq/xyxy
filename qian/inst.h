#ifndef QIAN_INST_H_
#define QIAN_INST_H_

#include "vector.h"
#include "chunk.h"
#include "status.h"
#include "logging.h"

namespace qian {

typedef enum {
  OP_RETURN,
  OP_CONSTANT,
  OP_NEGATE,
} OpCode;

class Inst {
 public:
  Inst() {}
  virtual ~Inst() {}

  typedef std::function<void(Inst*)> FuncDebugInfo;

  void Name(const string& name) { name_ = name; }
  string Name() { return name_; }

  void Length(uint8 length) { length = length; }
  uint8 Length() { return length; }

  void DebugInfo(const FuncDebugInfo& f) { debug_info_ = f; }
  void DebugInfo() { debug_info_(this); }

  void Opcode(uint8 opcode) { opcode_ = opcode; }
  uint8 Opcode() { return opcode_; }

  void Operand(Value operand) { operands_.Write(operand); }
  Vector<Value>* Operands() { return &operands_; }

 private:
  string name_;
  uint8 opcode_;
  uint8 length;
  Vector<Value> operands_;
  FuncDebugInfo debug_info_;
};

static Vector<Inst*>* GlobalInst() {
  static Vector<Inst*>* registry_;
  if (registry_ == nullptr) {
    registry_ = new Vector<Inst*> ();
  }
  return registry_;
}

Inst* DispathInst(Chunk* chunk, uint8 offset);

} // namespace qian


namespace register_inst {

struct InstDefWrapper {
  InstDefWrapper(const string& name) {
    inst = new ::qian::Inst();
    inst->Name(name);
    LOG(INFO) << "register inst..." << inst->Name();
    ::qian::GlobalInst()->Write(inst);
  }
  InstDefWrapper& Name(const string& name) {
    inst->Name(name);
    return *this;
  }
  InstDefWrapper& Length(uint8 len) {
    inst->Length(len);
    return *this;
  }
  InstDefWrapper& Opcode(uint8 opcode) {
    inst->Opcode(opcode);
    return *this;
  }
  InstDefWrapper& Operand(uint8 operand) {
    inst->Operand(operand);
    return *this;
  }
  InstDefWrapper& DebugInfo(const std::function<void(::qian::Inst*)>& f) {
    inst->DebugInfo(f);
    return *this;
  }
 private:
  ::qian::Inst* inst;
};

} // namespace register_inst

#define REGISTER_INST(name) REGISTER_INST_UNIQ_HELPER(__COUNTER__, name)
#define REGISTER_INST_UNIQ_HELPER(ctr, name) REGISTER_INST_UNIQ(ctr, name)
#define REGISTER_INST_UNIQ(ctr, name)                                 \
  static ::register_inst::InstDefWrapper register_inst##ctr           \
    __attribute__ ((unused)) = ::register_inst::InstDefWrapper(name)

#endif  // QIAN_INST_H_
