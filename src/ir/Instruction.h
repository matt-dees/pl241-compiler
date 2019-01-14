#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "Value.h"
#include <cstdint>
#include <string>
#include <vector>

namespace cs241c {
class BasicBlock;

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
class RetInstruction;
class EndInstruction;
class BranchInstruction;
class BranchNotEqualInstruction;
class BranchEqualInstruction;
class BranchLessThanEqualInstruction;
class BranchLessThanInstruction;
class BranchGreaterThanEqualInstruction;
class BranchGreaterThanInstruction;
class BasicBlock;
class CallInstruction;

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
  virtual void visit(RetInstruction *) = 0;
  virtual void visit(EndInstruction *) = 0;
  virtual void visit(BranchInstruction *) = 0;
  virtual void visit(BranchNotEqualInstruction *) = 0;
  virtual void visit(BranchEqualInstruction *) = 0;
  virtual void visit(BranchLessThanEqualInstruction *) = 0;
  virtual void visit(BranchLessThanInstruction *) = 0;
  virtual void visit(BranchGreaterThanEqualInstruction *) = 0;
  virtual void visit(BranchGreaterThanInstruction *) = 0;
  virtual void visit(CallInstruction *) = 0;
};

class Instruction : public Value {
public:
  explicit Instruction(std::vector<Value *> Params, BasicBlock *Owner,
                       std::string Name);

  virtual void visit(InstructionVisitor *V) = 0;
  std::string toString();
  BasicBlock *getOwner();

private:
  BasicBlock *Owner;
  std::vector<Value *> Params;
  std::string Name;
};

template <typename T> class VisitableInstruction : public Instruction {
public:
  explicit VisitableInstruction(std::vector<Value *> Params, BasicBlock *BB,
                                std::string Name);

  void visit(InstructionVisitor *V) override {
    V->visit(static_cast<T *>(this));
  }
};

class NegInstruction : public VisitableInstruction<NegInstruction> {
public:
  explicit NegInstruction(Value *X, BasicBlock *Owner = nullptr);
};

class AddInstruction : public VisitableInstruction<AddInstruction> {
public:
  explicit AddInstruction(Value *X, Value *Y, BasicBlock *Owner = nullptr);
};

class SubInstruction : public VisitableInstruction<SubInstruction> {
public:
  explicit SubInstruction(Value *X, Value *Y, BasicBlock *Owner = nullptr);
};

class MulInstruction : public VisitableInstruction<MulInstruction> {
public:
  explicit MulInstruction(Value *X, Value *Y, BasicBlock *Owner = nullptr);
};

class DivInstruction : public VisitableInstruction<DivInstruction> {
public:
  explicit DivInstruction(Value *X, Value *Y, BasicBlock *Owner = nullptr);
};

class CmpInstruction : public VisitableInstruction<CmpInstruction> {
public:
  explicit CmpInstruction(Value *X, Value *Y, BasicBlock *Owner = nullptr);
};

class AddaInstruction : public VisitableInstruction<AddaInstruction> {
public:
  explicit AddaInstruction(Value *X, Value *Y, BasicBlock *Owner = nullptr);
};

class LoadInstruction : public VisitableInstruction<LoadInstruction> {
public:
  explicit LoadInstruction(Value *Y, BasicBlock *Owner = nullptr);
};

class StoreInstruction : public VisitableInstruction<StoreInstruction> {
public:
  explicit StoreInstruction(Value *Y, Value *X, BasicBlock *Owner = nullptr);
};

class MoveInstruction : public VisitableInstruction<MoveInstruction> {
public:
  explicit MoveInstruction(Value *Y, Value *X, BasicBlock *Owner = nullptr);
};

class PhiInstruction : public VisitableInstruction<PhiInstruction> {
public:
  explicit PhiInstruction(const std::vector<Value *> &Values,
                          BasicBlock *Owner);
};

class CallInstruction : public VisitableInstruction<CallInstruction> {
public:
  explicit CallInstruction(Value *X, BasicBlock *Owner = nullptr);
};

class BasicBlockTerminator {
public:
  virtual std::vector<BasicBlock *> followingBlocks();
};

class RetInstruction : public VisitableInstruction<RetInstruction>,
                       public BasicBlockTerminator {
public:
  explicit RetInstruction(Value *X, BasicBlock *Owner = nullptr);
};

class EndInstruction : public VisitableInstruction<EndInstruction>,
                       public BasicBlockTerminator {
public:
  explicit EndInstruction(BasicBlock *Owner);
};

class BranchInstruction : public VisitableInstruction<BranchInstruction>,
                          public BasicBlockTerminator {
  BasicBlock *Target;

public:
  explicit BranchInstruction(BasicBlock *Y, BasicBlock *Owner = nullptr);
  std::vector<BasicBlock *> followingBlocks() override;
};

class ConditionalBlockTerminator : public BasicBlockTerminator {
  BasicBlock *Then;
  BasicBlock *Else;

public:
  ConditionalBlockTerminator(BasicBlock *Then, BasicBlock *Else);
  std::vector<BasicBlock *> followingBlocks() override;
};

class BranchNotEqualInstruction
    : public VisitableInstruction<BranchNotEqualInstruction>,
      public ConditionalBlockTerminator {
public:
  explicit BranchNotEqualInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                                     BasicBlock *Else,
                                     BasicBlock *Owner = nullptr);
};

class BranchEqualInstruction
    : public VisitableInstruction<BranchEqualInstruction>,
      public ConditionalBlockTerminator {
public:
  explicit BranchEqualInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                                  BasicBlock *Else,
                                  BasicBlock *Owner = nullptr);
};

class BranchLessThanEqualInstruction
    : public VisitableInstruction<BranchLessThanEqualInstruction>,
      public ConditionalBlockTerminator {
public:
  explicit BranchLessThanEqualInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                                          BasicBlock *Else, BasicBlock *Owner);
};

class BranchLessThanInstruction
    : public VisitableInstruction<BranchLessThanInstruction>,
      public ConditionalBlockTerminator {
public:
  explicit BranchLessThanInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                                     BasicBlock *Else,
                                     BasicBlock *Owner = nullptr);
};

class BranchGreaterThanEqualInstruction
    : public VisitableInstruction<BranchGreaterThanEqualInstruction>,
      public ConditionalBlockTerminator {
public:
  explicit BranchGreaterThanEqualInstruction(CmpInstruction *Cmp,
                                             BasicBlock *Then, BasicBlock *Else,
                                             BasicBlock *Owner);
};

class BranchGreaterThanInstruction
    : public VisitableInstruction<BranchGreaterThanInstruction>,
      public ConditionalBlockTerminator {
public:
  explicit BranchGreaterThanInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                                        BasicBlock *Else,
                                        BasicBlock *Owner = nullptr);
};
} // namespace cs241c

#endif
