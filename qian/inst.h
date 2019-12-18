#ifndef QIAN_INST_H_
#define QIAN_INST_H_

typedef enum {
  OP_CONSTANT,
  OP_RETURN,
} OpCode;

class Inst {
 public:
  Inst(uint8_t byte) : byte_(byte) {}
  virtual ~Inst();

  virtual void ShowInfo() const = 0;
  virtual uint8_t Length() const = 0;

 private:
  uint8_t byte_;
};

class ReturnInst : public Inst {
 public:
  ReturnInst(uint8_t byte) : Inst(byte);

  void ShowInfo() const override {
    cout << name_ << "\n";
  }

  uint8_t Length() const override {
    return 1;
  }

 private:
  const string name_ = "OP_RETURN";
};

// class ConstantInst : public Inst {

// };

#endif  // QIAN_INST_H_