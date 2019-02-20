#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "InstructionType.h"
#include "Value.h"
#include <array>
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
  std::array<Value *, 2> Args;

protected:
  virtual void updateArg(int Index, Value *NewVal);

public:
  Instruction(InstructionType, int Id, Value *Arg1);
  Instruction(InstructionType, int Id, Value *Arg1, Value *Arg2);

  BasicBlock *getOwner() const;
  std::vector<Value *> arguments() const;
  bool updateArgs(const std::unordered_map<Value *, Value *> &UpdateCtx);
  bool updateArgs(const SSAContext &SSAVarCtx);

  void setId(int Id);
  std::string name() const override;
  std::string toString() const override;
  bool operator==(const Instruction &other) const;

private:
  void checkArgs();

  friend class BasicBlock;
};

class MemoryInstruction : public Instruction {
protected:
  Variable *Object;

  MemoryInstruction(InstructionType, int Id, Variable *Object, Value *Arg1);
  MemoryInstruction(InstructionType, int Id, Variable *Object, Value *Arg1, Value *Arg2);

public:
  Variable *object() const;
};

class BasicBlockTerminator : public Instruction {
  virtual BasicBlock *target();

public:
  BasicBlockTerminator(InstructionType, int Id);
  BasicBlockTerminator(InstructionType, int Id, Value *Arg1);
  BasicBlockTerminator(InstructionType, int Id, Value *Arg1, Value *Arg2);

  friend class BasicBlock;
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
public:
  ConditionalBlockTerminator(InstructionType, int Id, CmpInstruction *Cmp, BasicBlock *Target);
  BasicBlock *target() override;
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
};

class StoreInstruction : public MemoryInstruction {
public:
  StoreInstruction(int Id, Variable *Object, Value *Y, AddaInstruction *Address);
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
  BasicBlock *target() override;

public:
  BraInstruction(int Id, BasicBlock *Y);
};
} // namespace cs241c

#endif
