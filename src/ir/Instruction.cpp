#include "Instruction.h"

using namespace cs241c;

Instruction::Instruction(const std::vector<Value *> &Params,
                         const BasicBlock *Owner)
    : Owner(Owner), Params(Params) {}

BasicBlock const *Instruction::getOwner() { return Owner; }

template <typename T>
VisitableInstruction<T>::VisitableInstruction(
    const std::vector<Value *> &Params, const BasicBlock *Owner)
    : Instruction(Params, Owner) {}

NegInstruction::NegInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<NegInstruction>(std::vector<Value *>({X}), Owner) {}

AddInstruction::AddInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<AddInstruction>(std::vector<Value *>({X, Y}),
                                           Owner) {}

SubInstruction::SubInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<SubInstruction>(std::vector<Value *>({X, Y}),
                                           Owner) {}

MulInstruction::MulInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<MulInstruction>(std::vector<Value *>({X, Y}),
                                           Owner) {}

DivInstruction::DivInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<DivInstruction>(std::vector<Value *>({X, Y}),
                                           Owner) {}

CmpInstruction::CmpInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<CmpInstruction>(std::vector<Value *>({X, Y}),
                                           Owner) {}

AddaInstruction::AddaInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<AddaInstruction>(std::vector<Value *>({X, Y}),
                                            Owner) {}

LoadInstruction::LoadInstruction(Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<LoadInstruction>(std::vector<Value *>({Y}), Owner) {}

StoreInstruction::StoreInstruction(Value *Y, Value *X, const BasicBlock *Owner)
    : VisitableInstruction<StoreInstruction>(std::vector<Value *>({Y, X}),
                                             Owner) {}

MoveInstruction::MoveInstruction(Value *Y, Value *X, const BasicBlock *Owner)
    : VisitableInstruction<MoveInstruction>(std::vector<Value *>({Y, X}),
                                            Owner) {}

PhiInstruction::PhiInstruction(const std::vector<Value *> &Values,
                               const BasicBlock *Owner)
    : VisitableInstruction<PhiInstruction>(Values, Owner) {}

RetInstruction::RetInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<RetInstruction>(std::vector<Value *>({X}), Owner) {}

EndInstruction::EndInstruction(const BasicBlock *Owner)
    : VisitableInstruction<EndInstruction>(std::vector<Value *>({}), Owner) {}

BranchInstruction::BranchInstruction(Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchInstruction>(std::vector<Value *>({Y}),
                                              Owner) {}

BranchNotEqualInstruction::BranchNotEqualInstruction(Value *X, Value *Y,
                                                     const BasicBlock *Owner)
    : VisitableInstruction<BranchNotEqualInstruction>(
          std::vector<Value *>({X, Y}), Owner) {}

BranchEqualInstruction::BranchEqualInstruction(Value *X, Value *Y,
                                               const BasicBlock *Owner)
    : VisitableInstruction<BranchEqualInstruction>(std::vector<Value *>({X, Y}),
                                                   Owner) {}

BranchLessThanEqualInstruction::BranchLessThanEqualInstruction(
    Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchLessThanEqualInstruction>(
          std::vector<Value *>({X, Y}), Owner) {}

BranchLessThanInstruction::BranchLessThanInstruction(Value *X, Value *Y,
                                                     const BasicBlock *Owner)
    : VisitableInstruction<BranchLessThanInstruction>(
          std::vector<Value *>({X, Y}), Owner) {}

BranchGreaterThanEqualInstruction::BranchGreaterThanEqualInstruction(
    Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchGreaterThanEqualInstruction>(
          std::vector<Value *>({X, Y}), Owner) {}

BranchGreaterThanInstruction::BranchGreaterThanInstruction(
    Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchGreaterThanInstruction>(
          std::vector<Value *>({X, Y}), Owner) {}

CallInstruction::CallInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<CallInstruction>(std::vector<Value *>({X}), Owner) {}