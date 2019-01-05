#ifndef CS241C_IR_INSTRUCTION_H
#define CS241C_IR_INSTRUCTION_H

#include "Value.h"

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
} // namespace cs241c

#endif
