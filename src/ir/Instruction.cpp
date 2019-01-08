#include "Instruction.h"

using namespace cs241c;

Instruction::Instruction(const std::vector<Value *> &Params) : Params(Params) {}

template <typename T>
VisitableInstruction<T>::VisitableInstruction(
    const std::vector<Value *> &Params)
    : Instruction(Params) {}

NegInstruction::NegInstruction(Value *X)
    : VisitableInstruction<NegInstruction>(std::vector<Value *>{X}) {}

AddInstruction::AddInstruction(Value *X, Value *Y)
    : VisitableInstruction<AddInstruction>(std::vector<Value *>{X, Y}) {}

SubInstruction::SubInstruction(Value *X, Value *Y)
    : VisitableInstruction<SubInstruction>(std::vector<Value *>({X, Y})) {}

MulInstruction::MulInstruction(Value *X, Value *Y)
    : VisitableInstruction<MulInstruction>(std::vector<Value *>({X, Y})) {}

DivInstruction::DivInstruction(Value *X, Value *Y)
    : VisitableInstruction<DivInstruction>(std::vector<Value *>({X, Y})) {}

CmpInstruction::CmpInstruction(Value *X, Value *Y)
    : VisitableInstruction<CmpInstruction>(std::vector<Value *>({X, Y})) {}

AddaInstruction::AddaInstruction(Value *X, Value *Y)
    : VisitableInstruction<AddaInstruction>(std::vector<Value *>({X, Y})) {}

LoadInstruction::LoadInstruction(Value *Y)
    : VisitableInstruction<LoadInstruction>(std::vector<Value *>({Y})) {}

StoreInstruction::StoreInstruction(Value *Y, Value *X)
    : VisitableInstruction<StoreInstruction>(std::vector<Value *>({Y, X})) {}

MoveInstruction::MoveInstruction(Value *Y, Value *X)
    : VisitableInstruction<MoveInstruction>(std::vector<Value *>({Y, X})) {}

PhiInstruction::PhiInstruction(const std::vector<Value *> &Values)
    : VisitableInstruction<PhiInstruction>(Values) {}

RetInstruction::RetInstruction(Value *X)
    : VisitableInstruction<RetInstruction>(std::vector<Value *>({X})) {}

EndInstruction::EndInstruction()
    : VisitableInstruction<EndInstruction>(std::vector<Value *>({})) {}

BranchInstruction::BranchInstruction(Value *Y)
    : VisitableInstruction<BranchInstruction>(std::vector<Value *>({Y})) {}

BranchNotEqualInstruction::BranchNotEqualInstruction(Value *X, Value *Y)
    : VisitableInstruction<BranchNotEqualInstruction>(
          std::vector<Value *>({X, Y})) {}

BranchEqualInstruction::BranchEqualInstruction(Value *X, Value *Y)
    : VisitableInstruction<BranchEqualInstruction>(
          std::vector<Value *>({X, Y})) {}

BranchLessThanEqualInstruction::BranchLessThanEqualInstruction(Value *X,
                                                               Value *Y)
    : VisitableInstruction<BranchLessThanEqualInstruction>(
          std::vector<Value *>({X, Y})) {}

BranchLessThanInstruction::BranchLessThanInstruction(Value *X, Value *Y)
    : VisitableInstruction<BranchLessThanInstruction>(
          std::vector<Value *>({X, Y})) {}

BranchGreaterThanEqualInstruction::BranchGreaterThanEqualInstruction(Value *X,
                                                                     Value *Y)
    : VisitableInstruction<BranchGreaterThanEqualInstruction>(
          std::vector<Value *>({X, Y})) {}

BranchGreaterThanInstruction::BranchGreaterThanInstruction(Value *X, Value *Y)
    : VisitableInstruction<BranchGreaterThanInstruction>(
          std::vector<Value *>({X, Y})) {}