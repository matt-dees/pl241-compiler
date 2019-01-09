#include "Instruction.h"

using namespace cs241c;

Instruction::Instruction(const std::vector<Value *> &Params,
                         const BasicBlock *Owner, const std::string &Name)
    : Owner(Owner), Params(Params), Name(Name) {}

BasicBlock const *Instruction::getOwner() { return Owner; }

const std::string Instruction::toString() {
  std::string ret;
  ret += Name;
  ret += " ";
  auto it = Params.begin();
  while (it != Params.end()) {
    ret += (*it)->toString();
    ret += ", ";
  }
  ret += (*it)->toString();
  return ret;
}

template <typename T>
VisitableInstruction<T>::VisitableInstruction(
    const std::vector<Value *> &Params, const BasicBlock *Owner,
    const std::string &Name)
    : Instruction(Params, Owner, Name) {}

SetInstruction::SetInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<SetInstruction>({X}, Owner, "set") {}

NegInstruction::NegInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<NegInstruction>({X}, Owner, "neg") {}

AddInstruction::AddInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<AddInstruction>({X, Y}, Owner, "add") {}

SubInstruction::SubInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<SubInstruction>({X, Y}, Owner, "sub") {}

MulInstruction::MulInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<MulInstruction>({X, Y}, Owner, "mul") {}

DivInstruction::DivInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<DivInstruction>({X, Y}, Owner, "div") {}

CmpInstruction::CmpInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<CmpInstruction>({X, Y}, Owner, "cmp") {}

AddaInstruction::AddaInstruction(Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<AddaInstruction>({X, Y}, Owner, "adda") {}

LoadInstruction::LoadInstruction(Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<LoadInstruction>({Y}, Owner, "load") {}

StoreInstruction::StoreInstruction(Value *Y, Value *X, const BasicBlock *Owner)
    : VisitableInstruction<StoreInstruction>({Y, X}, Owner, "store") {}

MoveInstruction::MoveInstruction(Value *Y, Value *X, const BasicBlock *Owner)
    : VisitableInstruction<MoveInstruction>({Y, X}, Owner, "mov") {}

PhiInstruction::PhiInstruction(const std::vector<Value *> &Values,
                               const BasicBlock *Owner)
    : VisitableInstruction<PhiInstruction>(Values, Owner, "phi") {}

RetInstruction::RetInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<RetInstruction>({X}, Owner, "ret") {}

EndInstruction::EndInstruction(const BasicBlock *Owner)
    : VisitableInstruction<EndInstruction>({}, Owner, "end") {}

BranchInstruction::BranchInstruction(Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchInstruction>({Y}, Owner, "bra") {}

BranchNotEqualInstruction::BranchNotEqualInstruction(Value *X, Value *Y,
                                                     const BasicBlock *Owner)
    : VisitableInstruction<BranchNotEqualInstruction>({X, Y}, Owner, "bne") {}

BranchEqualInstruction::BranchEqualInstruction(Value *X, Value *Y,
                                               const BasicBlock *Owner)
    : VisitableInstruction<BranchEqualInstruction>({X, Y}, Owner, "beq") {}

BranchLessThanEqualInstruction::BranchLessThanEqualInstruction(
    Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchLessThanEqualInstruction>({X, Y}, Owner,
                                                           "ble") {}

BranchLessThanInstruction::BranchLessThanInstruction(Value *X, Value *Y,
                                                     const BasicBlock *Owner)
    : VisitableInstruction<BranchLessThanInstruction>({X, Y}, Owner, "blt") {}

BranchGreaterThanEqualInstruction::BranchGreaterThanEqualInstruction(
    Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchGreaterThanEqualInstruction>({X, Y}, Owner,
                                                              "bge") {}

BranchGreaterThanInstruction::BranchGreaterThanInstruction(
    Value *X, Value *Y, const BasicBlock *Owner)
    : VisitableInstruction<BranchGreaterThanInstruction>({X, Y}, Owner, "bgt") {
}

CallInstruction::CallInstruction(Value *X, const BasicBlock *Owner)
    : VisitableInstruction<CallInstruction>({X}, Owner, "call") {}
