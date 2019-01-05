#include "Instruction.h"

using namespace cs241c;

UnaryInstruction::UnaryInstruction(Value *X) : X(X) {}
BinaryInstruction::BinaryInstruction(Value *X, Value *Y) : X(X), Y(Y) {}

NegInstruction::NegInstruction(Value *X) : UnaryInstruction(X) {}
SubInstruction::SubInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
MulInstruction::MulInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
DivInstruction::DivInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
CmpInstruction::CmpInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
