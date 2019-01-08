#include "Instruction.h"

using namespace cs241c;

UnaryInstruction::UnaryInstruction(Value *X) : X(X) {}
BinaryInstruction::BinaryInstruction(Value *X, Value *Y) : X(X), Y(Y) {}
NaryInstruction::NaryInstruction(const std::vector<cs241c::Value *> &Values)
    : Values(Values) {}

NegInstruction::NegInstruction(Value *X) : UnaryInstruction(X) {}
SubInstruction::SubInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
MulInstruction::MulInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
DivInstruction::DivInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
CmpInstruction::CmpInstruction(Value *X, Value *Y) : BinaryInstruction(X, Y) {}
AddaInstruction::AddaInstruction(Value *X, Value *Y)
    : BinaryInstruction(X, Y) {}
LoadInstruction::LoadInstruction(Value *Y) : UnaryInstruction(Y) {}
StoreInstruction::StoreInstruction(Value *Y, Value *X)
    : BinaryInstruction(Y, X) {}
MoveInstruction::MoveInstruction(Value *Y, Value *X)
    : BinaryInstruction(Y, X) {}
PhiInstruction::PhiInstruction(const std::vector<Value *> &Values)
    : NaryInstruction(Values) {}
RetInstruction::RetInstruction(Value *X) : UnaryInstruction(X) {}
EndInstruction::EndInstruction() {}
BranchInstruction::BranchInstruction(Value *Y) : UnaryInstruction(Y) {}
BranchNotEqualInstruction::BranchNotEqualInstruction(Value *X, Value *Y)
    : BinaryInstruction(X, Y) {}
BranchEqualInstruction::BranchEqualInstruction(Value *X, Value *Y)
    : BinaryInstruction(X, Y) {}
BranchLessThanEqualInstruction::BranchLessThanEqualInstruction(Value *X,
                                                               Value *Y)
    : BinaryInstruction(X, Y) {}

BranchLessThanInstruction::BranchLessThanInstruction(Value *X, Value *Y)
    : BinaryInstruction(X, Y) {}

BranchGreaterThanEqualInstruction::BranchGreaterThanEqualInstruction(Value *X,
                                                                     Value *Y)
    : BinaryInstruction(X, Y) {}
BranchGreaterThanInstruction::BranchGreaterThanInstruction(Value *X, Value *Y)
    : BinaryInstruction(X, Y) {}