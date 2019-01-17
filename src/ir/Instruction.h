#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "BasicBlock.h"
#include "Function.h"
#include "Value.h"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cs241c {
class Function;

class NegInstruction;
class AddInstruction;
class SubInstruction;
class MulInstruction;
class DivInstruction;
class CmpInstruction;
class AddaInstruction;
class LoadInstruction;
class StoreInstruction;
class MoveInstruction;
class PhiInstruction;
class CallInstruction;
class RetInstruction;
class EndInstruction;
class BraInstruction;
class BneInstruction;
class BeqInstruction;
class BltInstruction;
class BleInstruction;
class BgeInstruction;
class BgtInstruction;
class BasicBlock;

class InstructionVisitor {
public:
  virtual void visit(NegInstruction *) = 0;
  virtual void visit(AddInstruction *) = 0;
  virtual void visit(SubInstruction *) = 0;
  virtual void visit(MulInstruction *) = 0;
  virtual void visit(DivInstruction *) = 0;
  virtual void visit(CmpInstruction *) = 0;
  virtual void visit(AddaInstruction *) = 0;
  virtual void visit(LoadInstruction *) = 0;
  virtual void visit(StoreInstruction *) = 0;
  virtual void visit(MoveInstruction *) = 0;
  virtual void visit(PhiInstruction *) = 0;
  virtual void visit(CallInstruction *) = 0;
  virtual void visit(RetInstruction *) = 0;
  virtual void visit(EndInstruction *) = 0;
  virtual void visit(BraInstruction *) = 0;
  virtual void visit(BneInstruction *) = 0;
  virtual void visit(BeqInstruction *) = 0;
  virtual void visit(BltInstruction *) = 0;
  virtual void visit(BleInstruction *) = 0;
  virtual void visit(BgeInstruction *) = 0;
  virtual void visit(BgtInstruction *) = 0;
};

class Instruction : public Value {
private:
  BasicBlock *Owner{};

protected:
  virtual std::string_view getName() const = 0;

public:
  BasicBlock *getOwner() const;
  virtual std::vector<Value *> getArguments() const = 0;
  virtual void visit(InstructionVisitor *V) = 0;
  std::string toString() const override;

  friend class BasicBlock;
};

template <typename T> class BaseInstruction : public Instruction {
private:
  std::vector<Value *> Arguments;

protected:
  BaseInstruction(std::vector<Value *> &&Params);

public:
  std::vector<Value *> getArguments() const override;
  void visit(InstructionVisitor *V) override;
};

class BasicBlockTerminator : public Instruction {
private:
  std::vector<Value *> Arguments;

protected:
  BasicBlockTerminator(std::vector<Value *> &&Params);

public:
  std::vector<Value *> getArguments() const override;
  virtual std::vector<BasicBlock *> followingBlocks();
};

template <typename T>
class BaseBasicBlockTerminator : public BasicBlockTerminator {
protected:
  BaseBasicBlockTerminator(std::vector<Value *> &&Params);

public:
  void visit(InstructionVisitor *V) override;
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
protected:
  ConditionalBlockTerminator(CmpInstruction *Cmp, BasicBlock *Then,
                             BasicBlock *Else);

public:
  std::vector<BasicBlock *> followingBlocks() override;
};

template <typename T>
class BaseConditionalBlockTerminator : public ConditionalBlockTerminator {
protected:
  BaseConditionalBlockTerminator(CmpInstruction *Cmp, BasicBlock *Then,
                                 BasicBlock *Else);

public:
  void visit(InstructionVisitor *V) override;
};

class NegInstruction : public BaseInstruction<NegInstruction> {
public:
  explicit NegInstruction(Value *X);
  std::string_view getName() const override;
};

class AddInstruction : public BaseInstruction<AddInstruction> {
public:
  AddInstruction(Value *X, Value *Y);
  std::string_view getName() const override;
};

class SubInstruction : public BaseInstruction<SubInstruction> {
public:
  SubInstruction(Value *X, Value *Y);
  std::string_view getName() const override;
};

class MulInstruction : public BaseInstruction<MulInstruction> {
public:
  MulInstruction(Value *X, Value *Y);
  std::string_view getName() const override;
};

class DivInstruction : public BaseInstruction<DivInstruction> {
public:
  DivInstruction(Value *X, Value *Y);
  std::string_view getName() const override;
};

class CmpInstruction : public BaseInstruction<CmpInstruction> {
public:
  CmpInstruction(Value *X, Value *Y);
  std::string_view getName() const override;
};

class AddaInstruction : public BaseInstruction<AddaInstruction> {
public:
  AddaInstruction(Value *X, Value *Y);
  std::string_view getName() const override;
};

class LoadInstruction : public BaseInstruction<LoadInstruction> {
public:
  explicit LoadInstruction(Value *Y);
  std::string_view getName() const override;
};

class StoreInstruction : public BaseInstruction<StoreInstruction> {
public:
  StoreInstruction(Value *Y, Value *X);
  std::string_view getName() const override;
};

class MoveInstruction : public BaseInstruction<MoveInstruction> {
public:
  MoveInstruction(Value *Y, Value *X);
  std::string_view getName() const override;
};

class PhiInstruction : public BaseInstruction<PhiInstruction> {
public:
  explicit PhiInstruction(Value *X1, Value *X2);
  std::string_view getName() const override;
};

class CallInstruction : public Instruction {
  Function *Target;
  std::vector<Value *> Arguments;

public:
  CallInstruction(Function *Target, std::vector<Value *> Arguments);

  std::string_view getName() const override;
  std::vector<Value *> getArguments() const override;

  void visit(InstructionVisitor *V) override;
};

class RetInstruction : public BaseBasicBlockTerminator<RetInstruction> {
public:
  RetInstruction();
  explicit RetInstruction(Value *X);

  std::string_view getName() const override;
};

class EndInstruction : public BaseBasicBlockTerminator<EndInstruction> {
public:
  EndInstruction();
  std::string_view getName() const override;
};

class BraInstruction : public BaseBasicBlockTerminator<BraInstruction> {
public:
  explicit BraInstruction(BasicBlock *Y);
  std::string_view getName() const override;
  std::vector<BasicBlock *> followingBlocks() override;
};

class BneInstruction : public BaseConditionalBlockTerminator<BneInstruction> {
public:
  explicit BneInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                          BasicBlock *Else);
  std::string_view getName() const override;
};

class BeqInstruction : public BaseConditionalBlockTerminator<BeqInstruction> {
public:
  explicit BeqInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                          BasicBlock *Else);
  std::string_view getName() const override;
};

class BltInstruction : public BaseConditionalBlockTerminator<BltInstruction> {
public:
  explicit BltInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                          BasicBlock *Else);
  std::string_view getName() const override;
};

class BleInstruction : public BaseConditionalBlockTerminator<BleInstruction> {
public:
  explicit BleInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                          BasicBlock *Else);
  std::string_view getName() const override;
};

class BgeInstruction : public BaseConditionalBlockTerminator<BgeInstruction> {
public:
  explicit BgeInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                          BasicBlock *Else);
  std::string_view getName() const override;
};

class BgtInstruction : public BaseConditionalBlockTerminator<BgtInstruction> {
public:
  explicit BgtInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                          BasicBlock *Else);
  std::string_view getName() const override;
};
} // namespace cs241c

#endif
