#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "InstructionType.h"
#include "Value.h"
#include <array>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace cs241c {
class BasicBlock;
class Function;
class SSAContext;
class Variable;

class ValueRef {
public:
  ValueType ValTy;

  union {
    Value *Ptr;
    int Id;
  } R{};

  ValueRef();
  ValueRef(Value *Ptr);
  ValueRef(ValueType Ty, int Id);

  bool isUndef();

  operator Value *() const;
  Value *operator->() const;

  bool operator==(ValueRef Other) const;
  bool operator<(ValueRef Other) const;
};

class Instruction : public Value {
public:
  const InstructionType InstrT;

private:
  int Id;
  BasicBlock *Owner{};
  Variable *Storage{};
  std::array<ValueRef, 2> Args;

protected:
  virtual void updateArg(int Index, ValueRef NewVal);

public:
  Instruction(InstructionType, int Id, ValueRef Arg1);
  Instruction(InstructionType, int Id, ValueRef Arg1, ValueRef Arg2);

  BasicBlock *getOwner() const;
  Variable *&storage();
  std::vector<ValueRef> arguments() const;
  bool updateArgs(const std::map<ValueRef, ValueRef> &UpdateCtx);
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

public:
  MemoryInstruction(int Id, InstructionType, Variable *Object, Value *Arg1);
  MemoryInstruction(int Id, InstructionType, Variable *Object, Value *Arg1, Value *Arg2);

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
  ConditionalBlockTerminator(InstructionType, int Id, Instruction *Cmp, BasicBlock *Target);
  BasicBlock *target() override;
};

class MoveInstruction : public Instruction {
public:
  MoveInstruction(int Id, ValueRef Y, ValueRef X);

  void updateArgs(Value *NewTarget, Value *NewSource);
  Value *source() const;
  Value *target() const;
};

class BraInstruction : public BasicBlockTerminator {
  BasicBlock *target() override;

public:
  BraInstruction(int Id, BasicBlock *Y);
};
} // namespace cs241c

#endif
