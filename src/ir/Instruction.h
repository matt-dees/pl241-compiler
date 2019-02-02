#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "Value.h"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cs241c {
class BasicBlock;
class Function;
class SSAContext;
class Variable;

class AddaInstruction;
class CmpInstruction;

class Instruction : public Value {
private:
  int Id;
  BasicBlock *Owner{};
  std::vector<Value *> Arguments;

protected:
  Instruction(int Id, std::vector<Value *> &&Arguments);
  virtual std::string_view mnemonic() const = 0;

public:
  BasicBlock *getOwner() const;
  std::vector<Value *> &arguments();
  const std::vector<Value *> &arguments() const;
  virtual void updateArg(unsigned long Index, Value *NewVal);

  virtual void argsToSSA(SSAContext &);
  void setId(int Id);
  std::string name() const override;
  std::string toString() const override;

  friend class BasicBlock;
};

class MemoryInstruction : public Instruction {
protected:
  Variable *Object;

  MemoryInstruction(int Id, Variable *Object, std::vector<Value *> &&Arguments);

public:
  Variable *object() const;
};

class BasicBlockTerminator : public Instruction {
protected:
  BasicBlockTerminator(int Id, std::vector<Value *> &&Arguments);

public:
  void argsToSSA(SSAContext &SSACtx) override;
  virtual std::vector<BasicBlock *> followingBlocks();
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
protected:
  ConditionalBlockTerminator(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                             BasicBlock *Else);

public:
  std::vector<BasicBlock *> followingBlocks() override;
};

class NegInstruction : public Instruction {
public:
  NegInstruction(int Id, Value *X);
  std::string_view mnemonic() const override;
};

class AddInstruction : public Instruction {
public:
  AddInstruction(int Id, Value *X, Value *Y);
  std::string_view mnemonic() const override;
};

class SubInstruction : public Instruction {
public:
  SubInstruction(int Id, Value *X, Value *Y);
  std::string_view mnemonic() const override;
};

class MulInstruction : public Instruction {
public:
  MulInstruction(int Id, Value *X, Value *Y);
  std::string_view mnemonic() const override;
};

class DivInstruction : public Instruction {
public:
  DivInstruction(int Id, Value *X, Value *Y);
  std::string_view mnemonic() const override;
};

class CmpInstruction : public Instruction {
public:
  CmpInstruction(int Id, Value *X, Value *Y);
  std::string_view mnemonic() const override;
};

class AddaInstruction : public Instruction {
public:
  AddaInstruction(int Id, Value *X, Value *Y);
  std::string_view mnemonic() const override;
};

class LoadInstruction : public MemoryInstruction {
public:
  LoadInstruction(int Id, Variable *Object, AddaInstruction *Address);
  void updateArg(unsigned long Index, Value *NewVal) override;
  std::string_view mnemonic() const override;
};

class StoreInstruction : public MemoryInstruction {
public:
  StoreInstruction(int Id, Variable *Object, Value *Y,
                   AddaInstruction *Address);
  void updateArg(unsigned long Index, Value *NewVal) override;
  std::string_view mnemonic() const override;
};

class MoveInstruction : public Instruction {
public:
  MoveInstruction(int Id, Value *Y, Value *X);
  std::string_view mnemonic() const override;
  void updateArgs(Value *NewTarget, Value *NewSource);
  Value *Target;
  Value *Source;
};

class PhiInstruction : public Instruction {
public:
  PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2);
  std::string_view mnemonic() const override;
  Variable *Target;
};

class CallInstruction : public Instruction {
public:
  CallInstruction(int Id, Function *Target, std::vector<Value *> Arguments);
  std::string_view mnemonic() const override;
};

class RetInstruction : public BasicBlockTerminator {
public:
  RetInstruction(int Id);
  RetInstruction(int Id, Value *X);

  std::string_view mnemonic() const override;
};

class EndInstruction : public BasicBlockTerminator {
public:
  EndInstruction(int Id);
  std::string_view mnemonic() const override;
};

class BraInstruction : public BasicBlockTerminator {
public:
  BraInstruction(int Id, BasicBlock *Y);
  std::string_view mnemonic() const override;
  std::vector<BasicBlock *> followingBlocks() override;
};

class BneInstruction : public ConditionalBlockTerminator {
public:
  BneInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BeqInstruction : public ConditionalBlockTerminator {
public:
  BeqInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BltInstruction : public ConditionalBlockTerminator {
public:
  BltInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BleInstruction : public ConditionalBlockTerminator {
public:
  BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BgeInstruction : public ConditionalBlockTerminator {
public:
  BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BgtInstruction : public ConditionalBlockTerminator {
public:
  BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class ReadInstruction : public Instruction {
public:
  ReadInstruction(int Id);
  std::string_view mnemonic() const override;
};

class WriteInstruction : public Instruction {
public:
  WriteInstruction(int Id, Value *X);
  std::string_view mnemonic() const override;
};

class WriteNLInstruction : public Instruction {
public:
  WriteNLInstruction(int Id);
  std::string_view mnemonic() const override;
};
} // namespace cs241c

#endif
