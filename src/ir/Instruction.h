#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "Value.h"
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
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
  virtual void updateArg(unsigned long Index, Value *NewVal);

public:
  BasicBlock *getOwner() const;
  std::vector<Value *> &arguments();
  const std::vector<Value *> &arguments() const;
  void updateArgs(const std::unordered_map<Value *, Value *> &UpdateCtx);
  void updateArgs(const SSAContext &SSAVarCtx);

  void setId(int Id);
  virtual std::string_view mnemonic() const = 0;
  std::string name() const override;
  virtual bool isPreLive() const;
  std::string toString() const override;
  bool operator==(const Instruction &other) const;

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
  virtual std::vector<BasicBlock *> followingBlocks();
  bool isPreLive() const override;
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
protected:
  ConditionalBlockTerminator(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);

public:
  std::vector<BasicBlock *> followingBlocks() override;
  void updateTarget(BasicBlock *OldTarget, BasicBlock *NewTarget);
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
  StoreInstruction(int Id, Variable *Object, Value *Y, AddaInstruction *Address);
  void updateArg(unsigned long Index, Value *NewVal) override;
  std::string_view mnemonic() const override;
  bool isPreLive() const override;
};

class MoveInstruction : public Instruction {
public:
  MoveInstruction(int Id, Value *Y, Value *X);
  std::string_view mnemonic() const override;
  void updateArgs(Value *NewTarget, Value *NewSource);
  bool isPreLive() const override;
  Value *source() const;
  Value *target() const;
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
  bool isPreLive() const override;
};

class RetInstruction : public BasicBlockTerminator {
public:
  explicit RetInstruction(int Id);
  RetInstruction(int Id, Value *X);

  std::string_view mnemonic() const override;
};

class EndInstruction : public BasicBlockTerminator {
public:
  explicit EndInstruction(int Id);
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
  BneInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BeqInstruction : public ConditionalBlockTerminator {
public:
  BeqInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BltInstruction : public ConditionalBlockTerminator {
public:
  BltInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BleInstruction : public ConditionalBlockTerminator {
public:
  BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BgeInstruction : public ConditionalBlockTerminator {
public:
  BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class BgtInstruction : public ConditionalBlockTerminator {
public:
  BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  std::string_view mnemonic() const override;
};

class ReadInstruction : public Instruction {
public:
  explicit ReadInstruction(int Id);
  std::string_view mnemonic() const override;
  bool isPreLive() const override;
};

class WriteInstruction : public Instruction {
public:
  WriteInstruction(int Id, Value *X);
  std::string_view mnemonic() const override;
  bool isPreLive() const override;
};

class WriteNLInstruction : public Instruction {
public:
  explicit WriteNLInstruction(int Id);
  std::string_view mnemonic() const override;
  bool isPreLive() const override;
};
} // namespace cs241c

#endif
