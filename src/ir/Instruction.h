#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "BasicBlock.h"
#include "Function.h"
#include "SSAContext.h"
#include "Value.h"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cs241c {
class BasicBlock;
class Function;

class CmpInstruction;

class Instruction : public Value {
private:
  int Id;
  BasicBlock *Owner{};

protected:
  Instruction(int Id);
  virtual std::string_view getName() const = 0;

public:
  BasicBlock *getOwner() const;
  virtual std::vector<Value *> getArguments() const = 0;
  virtual void updateArg(const unsigned long Index, Value *NewVal) = 0;

  virtual void argsToSSA(SSAContext &) {}
  void setId(int NewID);
  std::string id() const override;
  std::string toString() const override;

  friend class BasicBlock;
};

class BaseInstruction : public Instruction {
protected:
  BaseInstruction(int Id, std::vector<Value *> &&Params);
  std::vector<Value *> Arguments;

public:
  void updateArg(const unsigned long Index, Value *NewVal) override;
  void argsToSSA(SSAContext &SSACtx) override;
  std::vector<Value *> getArguments() const override;
};

class BasicBlockTerminator : public Instruction {
protected:
  std::vector<Value *> Arguments;
  BasicBlockTerminator(int Id, std::vector<Value *> &&Params);

public:
  void updateArg(const unsigned long Index, Value *NewVal) override;
  void argsToSSA(SSAContext &SSACtx) override;
  std::vector<Value *> getArguments() const override;
  virtual std::vector<BasicBlock *> followingBlocks();
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
protected:
  ConditionalBlockTerminator(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                             BasicBlock *Else);

public:
  std::vector<BasicBlock *> followingBlocks() override;
};

class NegInstruction : public BaseInstruction {
public:
  NegInstruction(int Id, Value *X);
  std::string_view getName() const override;
};

class AddInstruction : public BaseInstruction {
public:
  AddInstruction(int Id, Value *X, Value *Y);
  std::string_view getName() const override;
};

class SubInstruction : public BaseInstruction {
public:
  SubInstruction(int Id, Value *X, Value *Y);
  std::string_view getName() const override;
};

class MulInstruction : public BaseInstruction {
public:
  MulInstruction(int Id, Value *X, Value *Y);
  std::string_view getName() const override;
};

class DivInstruction : public BaseInstruction {
public:
  DivInstruction(int Id, Value *X, Value *Y);
  std::string_view getName() const override;
};

class CmpInstruction : public BaseInstruction {
public:
  CmpInstruction(int Id, Value *X, Value *Y);
  std::string_view getName() const override;
};

class AddaInstruction : public BaseInstruction {
public:
  AddaInstruction(int Id, Value *X, Value *Y);
  std::string_view getName() const override;
};

class LoadInstruction : public BaseInstruction {
public:
  LoadInstruction(int Id, Value *Y);
  std::string_view getName() const override;
};

class StoreInstruction : public BaseInstruction {
public:
  StoreInstruction(int Id, Value *Y, Value *X);
  std::string_view getName() const override;
};

class MoveInstruction : public BaseInstruction {
public:
  MoveInstruction(int Id, Value *Y, Value *X);
  std::string_view getName() const override;
  void updateArgs(Value *NewTarget, Value *NewSource);
  Value *Target;
  Value *Source;
};

class PhiInstruction : public BaseInstruction {
public:
  PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2);
  std::string_view getName() const override;
  Variable *Target;
};

class CallInstruction : public Instruction {
  Function *Target;
  std::vector<Value *> Arguments;

public:
  CallInstruction(int Id, Function *Target, std::vector<Value *> Arguments);

  std::string_view getName() const override;
  std::vector<Value *> getArguments() const override;
  void updateArg(const unsigned long Index, Value *NewVal) override;
};

class RetInstruction : public BasicBlockTerminator {
public:
  RetInstruction(int Id);
  RetInstruction(int Id, Value *X);

  std::string_view getName() const override;
};

class EndInstruction : public BasicBlockTerminator {
public:
  EndInstruction(int Id);
  std::string_view getName() const override;
};

class BraInstruction : public BasicBlockTerminator {
public:
  BraInstruction(int Id, BasicBlock *Y);
  std::string_view getName() const override;
  std::vector<BasicBlock *> followingBlocks() override;
};

class BneInstruction : public ConditionalBlockTerminator {
public:
  BneInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view getName() const override;
};

class BeqInstruction : public ConditionalBlockTerminator {
public:
  BeqInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view getName() const override;
};

class BltInstruction : public ConditionalBlockTerminator {
public:
  BltInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view getName() const override;
};

class BleInstruction : public ConditionalBlockTerminator {
public:
  BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view getName() const override;
};

class BgeInstruction : public ConditionalBlockTerminator {
public:
  BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view getName() const override;
};

class BgtInstruction : public ConditionalBlockTerminator {
public:
  BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                 BasicBlock *Else);
  std::string_view getName() const override;
};

class ReadInstruction : public BaseInstruction {
public:
  ReadInstruction(int Id);
  std::string_view getName() const override;
};

class WriteInstruction : public BaseInstruction {
public:
  WriteInstruction(int Id, Value *X);
  std::string_view getName() const override;
};

class WriteNLInstruction : public BaseInstruction {
public:
  WriteNLInstruction(int Id);
  std::string_view getName() const override;
};
} // namespace cs241c

#endif
