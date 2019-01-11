#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "BasicBlock.h"
#include "BasicBlockTerminator.h"
#include "Value.h"
#include <cstdint>
#include <string>
#include <vector>

namespace cs241c {
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
  virtual void visit(NegInstruction *I) = 0;
  virtual void visit(AddInstruction *I) = 0;
  virtual void visit(SubInstruction *I) = 0;
  virtual void visit(MulInstruction *I) = 0;
  virtual void visit(DivInstruction *I) = 0;
  virtual void visit(CmpInstruction *I) = 0;
  virtual void visit(AddaInstruction *I) = 0;
  virtual void visit(LoadInstruction *I) = 0;
  virtual void visit(StoreInstruction *I) = 0;
  virtual void visit(MoveInstruction *I) = 0;
  virtual void visit(PhiInstruction *I) = 0;
  virtual void visit(RetInstruction *I) = 0;
  virtual void visit(EndInstruction *I) = 0;
  virtual void visit(BranchInstruction *I) = 0;
  virtual void visit(BranchNotEqualInstruction *I) = 0;
  virtual void visit(BranchEqualInstruction *I) = 0;
  virtual void visit(BranchLessThanEqualInstruction *I) = 0;
  virtual void visit(BranchLessThanInstruction *I) = 0;
  virtual void visit(BranchGreaterThanEqualInstruction *I) = 0;
  virtual void visit(BranchGreaterThanInstruction *I) = 0;
  virtual void visit(CallInstruction *I) = 0;
};

class Instruction {
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
  explicit NegInstruction(Value *X, BasicBlock *Owner);
};

class AddInstruction : public VisitableInstruction<AddInstruction> {
public:
  explicit AddInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class SubInstruction : public VisitableInstruction<SubInstruction> {
public:
  explicit SubInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class MulInstruction : public VisitableInstruction<MulInstruction> {
public:
  explicit MulInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class DivInstruction : public VisitableInstruction<DivInstruction> {
public:
  explicit DivInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class CmpInstruction : public VisitableInstruction<CmpInstruction> {
public:
  explicit CmpInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class AddaInstruction : public VisitableInstruction<AddaInstruction> {
public:
  explicit AddaInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class LoadInstruction : public VisitableInstruction<LoadInstruction> {
public:
  explicit LoadInstruction(Value *Y, BasicBlock *Owner);
};

class StoreInstruction : public VisitableInstruction<StoreInstruction> {
public:
  explicit StoreInstruction(Value *Y, Value *X, BasicBlock *Owner);
};

class MoveInstruction : public VisitableInstruction<MoveInstruction> {
public:
  explicit MoveInstruction(Value *Y, Value *X, BasicBlock *Owner);
};

class PhiInstruction : public VisitableInstruction<PhiInstruction> {
public:
  explicit PhiInstruction(const std::vector<Value *> &Values,
                          BasicBlock *Owner);
};

class RetInstruction : public VisitableInstruction<RetInstruction>,
                       public BasicBlockTerminator {
public:
  explicit RetInstruction(Value *X, BasicBlock *Owner);
};

class EndInstruction : public VisitableInstruction<EndInstruction>,
                       public BasicBlockTerminator {
public:
  explicit EndInstruction(BasicBlock *Owner);
};

class BranchInstruction : public VisitableInstruction<BranchInstruction>,
                          public BasicBlockTerminator {
public:
  explicit BranchInstruction(Value *Y, BasicBlock *Owner);
};

class BranchNotEqualInstruction
    : public VisitableInstruction<BranchNotEqualInstruction>,
      public BasicBlockTerminator {
public:
  explicit BranchNotEqualInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class BranchEqualInstruction
    : public VisitableInstruction<BranchEqualInstruction>,
      public BasicBlockTerminator {
public:
  explicit BranchEqualInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class BranchLessThanEqualInstruction
    : public VisitableInstruction<BranchLessThanEqualInstruction>,
      public BasicBlockTerminator {
public:
  explicit BranchLessThanEqualInstruction(Value *X, Value *Y,
                                          BasicBlock *Owner);
};

class BranchLessThanInstruction
    : public VisitableInstruction<BranchLessThanInstruction>,
      public BasicBlockTerminator {
public:
  explicit BranchLessThanInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class BranchGreaterThanEqualInstruction
    : public VisitableInstruction<BranchGreaterThanEqualInstruction>,
      public BasicBlockTerminator {
public:
  explicit BranchGreaterThanEqualInstruction(Value *X, Value *Y,
                                             BasicBlock *Owner);
};

class BranchGreaterThanInstruction
    : public VisitableInstruction<BranchGreaterThanInstruction>,
      public BasicBlockTerminator {
public:
  explicit BranchGreaterThanInstruction(Value *X, Value *Y, BasicBlock *Owner);
};

class CallInstruction : public VisitableInstruction<CallInstruction>,
                        public BasicBlockTerminator {
public:
  explicit CallInstruction(Value *X, BasicBlock *Owner);
};
} // namespace cs241c

#endif
