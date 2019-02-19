#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "InstructionType.h"
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
public:
  const InstructionType InstrT;

private:
  int Id;
  BasicBlock *Owner{};
  std::vector<Value *> Arguments;

protected:
  virtual void updateArg(unsigned long Index, Value *NewVal);

public:
  Instruction(InstructionType, int Id, std::vector<Value *> &&Arguments);

  BasicBlock *getOwner() const;
  std::vector<Value *> &arguments();
  const std::vector<Value *> &arguments() const;
  bool updateArgs(const std::unordered_map<Value *, Value *> &UpdateCtx);
  bool updateArgs(const SSAContext &SSAVarCtx);

  void setId(int Id);
  std::string name() const override;
  std::string toString() const override;
  bool operator==(const Instruction &other) const;

  friend class BasicBlock;
};

class MemoryInstruction : public Instruction {
protected:
  Variable *Object;

  MemoryInstruction(InstructionType, int Id, Variable *Object, std::vector<Value *> &&Arguments);

public:
  Variable *object() const;
};

class BasicBlockTerminator : public Instruction {
  virtual std::vector<BasicBlock *> followingBlocks();

public:
  BasicBlockTerminator(InstructionType, int Id, std::vector<Value *> &&Arguments);

  friend class BasicBlock;
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
  std::vector<BasicBlock *> followingBlocks() override;

public:
  ConditionalBlockTerminator(InstructionType, int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else);
  BasicBlock *elseBlock() const;
  void updateTarget(BasicBlock *OldTarget, BasicBlock *NewTarget);
};

class CmpInstruction : public Instruction {
public:
  CmpInstruction(int Id, Value *X, Value *Y);
};

class AddaInstruction : public Instruction {
public:
  AddaInstruction(int Id, Value *X, Value *Y);
};

class LoadInstruction : public MemoryInstruction {
public:
  LoadInstruction(int Id, Variable *Object, AddaInstruction *Address);
  void updateArg(unsigned long Index, Value *NewVal) override;
};

class StoreInstruction : public MemoryInstruction {
public:
  StoreInstruction(int Id, Variable *Object, Value *Y, AddaInstruction *Address);
  void updateArg(unsigned long Index, Value *NewVal) override;
};

class MoveInstruction : public Instruction {
public:
  MoveInstruction(int Id, Value *Y, Value *X);

  void updateArgs(Value *NewTarget, Value *NewSource);
  Value *source() const;
  Value *target() const;
};

class PhiInstruction : public Instruction {
public:
  PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2);

  Variable *Target;
};

class BraInstruction : public BasicBlockTerminator {
  std::vector<BasicBlock *> followingBlocks() override;

public:
  BraInstruction(int Id, BasicBlock *Y);

  void updateTarget(BasicBlock *NewTarget);
};
} // namespace cs241c

#endif
