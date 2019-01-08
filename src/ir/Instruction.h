#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "BasicBlockTerminator.h"
#include "Value.h"
#include <vector>

namespace cs241c {
class NegInstruction;
class AddInstruction;
class SubInstruction;
class MulInstruction;
class DivInstruction;
class CmpInstruction;

class InstructionVisitor {
public:
  virtual void visit(NegInstruction *I) = 0;
  virtual void visit(AddInstruction *I) = 0;
  virtual void visit(SubInstruction *I) = 0;
  virtual void visit(MulInstruction *I) = 0;
  virtual void visit(DivInstruction *I) = 0;
  virtual void visit(CmpInstruction *I) = 0;
};

class Instruction : public Value {
  virtual void visit(InstructionVisitor *V) = 0;
};

template <typename T> class VisitableInstruction : public Instruction {
  void visit(InstructionVisitor *V) override {
    V->visit(static_cast<T *>(this));
  }
};

class UnaryInstruction {
public:
  Value *X;

  explicit UnaryInstruction(Value *X);
};

class BinaryInstruction {
public:
  Value *X;
  Value *Y;

  explicit BinaryInstruction(Value *X, Value *Y);
};

class NaryInstruction {
public:
  const std::vector<Value *> Values;

  explicit NaryInstruction(const std::vector<Value *> &Values);
};

class NegInstruction : public VisitableInstruction<NegInstruction>,
                       public UnaryInstruction {
public:
  explicit NegInstruction(Value *X);
};

class AddInstruction : public VisitableInstruction<AddInstruction>,
                       public BinaryInstruction {
public:
  explicit AddInstruction(Value *X, Value *Y);
};

class SubInstruction : public VisitableInstruction<SubInstruction>,
                       public BinaryInstruction {
public:
  explicit SubInstruction(Value *X, Value *Y);
};

class MulInstruction : public VisitableInstruction<MulInstruction>,
                       public BinaryInstruction {
public:
  explicit MulInstruction(Value *X, Value *Y);
};

class DivInstruction : public VisitableInstruction<DivInstruction>,
                       public BinaryInstruction {
public:
  explicit DivInstruction(Value *X, Value *Y);
};

class CmpInstruction : public VisitableInstruction<CmpInstruction>,
                       public BinaryInstruction {
public:
  explicit CmpInstruction(Value *X, Value *Y);
};

class AddaInstruction : public VisitableInstruction<AddaInstruction>,
                        public BinaryInstruction {
public:
  explicit AddaInstruction(Value *X, Value *Y);
};

class LoadInstruction : public VisitableInstruction<LoadInstruction>,
                        public UnaryInstruction {
public:
  explicit LoadInstruction(Value *Y);
};

class StoreInstruction : public VisitableInstruction<StoreInstruction>,
                         public BinaryInstruction {
public:
  explicit StoreInstruction(Value *Y, Value *X);
};

class MoveInstruction : public VisitableInstruction<MoveInstruction>,
                        public BinaryInstruction {
public:
  explicit MoveInstruction(Value *Y, Value *X);
};

class PhiInstruction : public VisitableInstruction<PhiInstruction>,
                       public NaryInstruction {
public:
  explicit PhiInstruction(const std::vector<Value *> &Values);
};

class RetInstruction : public VisitableInstruction<RetInstruction>,
                       public UnaryInstruction,
                       public BasicBlockTerminator {
public:
  explicit RetInstruction(Value *X);
};

class EndInstruction : public VisitableInstruction<EndInstruction>,
                       public BasicBlockTerminator {
public:
  explicit EndInstruction();
};

class BranchInstruction : public VisitableInstruction<BranchInstruction>,
                          public BasicBlockTerminator,
                          public UnaryInstruction {
public:
  explicit BranchInstruction(Value *Y);
};

class BranchNotEqualInstruction
    : public VisitableInstruction<BranchNotEqualInstruction>,
      public BasicBlockTerminator,
      public BinaryInstruction {
public:
  explicit BranchNotEqualInstruction(Value *X, Value *Y);
}; // namespace cs241c

class BranchEqualInstruction
    : public VisitableInstruction<BranchEqualInstruction>,
      public BasicBlockTerminator,
      public BinaryInstruction {
public:
  explicit BranchEqualInstruction(Value *X, Value *Y);
};

class BranchLessThanEqualInstruction
    : public VisitableInstruction<BranchLessThanEqualInstruction>,
      public BasicBlockTerminator,
      public BinaryInstruction {
public:
  explicit BranchLessThanEqualInstruction(Value *X, Value *Y);
};

class BranchLessThanInstruction
    : public VisitableInstruction<BranchLessThanInstruction>,
      public BasicBlockTerminator,
      public BinaryInstruction {
public:
  explicit BranchLessThanInstruction(Value *X, Value *Y);
};

class BranchGreaterThanEqualInstruction
    : public VisitableInstruction<BranchGreaterThanEqualInstruction>,
      public BasicBlockTerminator,
      public BinaryInstruction {
public:
  explicit BranchGreaterThanEqualInstruction(Value *X, Value *Y);
};

class BranchGreaterThanInstruction
    : public VisitableInstruction<BranchGreaterThanInstruction>,
      public BasicBlockTerminator,
      public BinaryInstruction {
public:
  explicit BranchGreaterThanInstruction(Value *X, Value *Y);
};
} // namespace cs241c

#endif
