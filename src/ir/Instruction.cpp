#include "Instruction.h"

using namespace cs241c;

Instruction::Instruction(std::vector<Value *> Params, BasicBlock *Owner,
                         std::string Name)
    : Value(""), Owner(Owner), Params(std::move(Params)),
      Name(std::move(Name)) {}

BasicBlock *Instruction::getOwner() { return Owner; }

std::string Instruction::toString() {
  std::string ret;
  ret += Name;
  if (Params.size() == 0) {
    return ret;
  }
  auto it = Params.begin();
  ret += " ";
  while (it < Params.end() - 1) {
    ret += (*it)->toString();
    ret += ", ";
    it++;
  }

  ret += (*it)->toString();
  return ret;
}

template <typename T>
VisitableInstruction<T>::VisitableInstruction(std::vector<Value *> Params,
                                              BasicBlock *Owner,
                                              std::string Name)
    : Instruction(std::move(Params), Owner, std::move(Name)) {}

NegInstruction::NegInstruction(Value *X, BasicBlock *Owner)
    : VisitableInstruction<NegInstruction>({X}, Owner, "neg") {}

AddInstruction::AddInstruction(Value *X, Value *Y, BasicBlock *Owner)
    : VisitableInstruction<AddInstruction>({X, Y}, Owner, "add") {}

SubInstruction::SubInstruction(Value *X, Value *Y, BasicBlock *Owner)
    : VisitableInstruction<SubInstruction>({X, Y}, Owner, "sub") {}

MulInstruction::MulInstruction(Value *X, Value *Y, BasicBlock *Owner)
    : VisitableInstruction<MulInstruction>({X, Y}, Owner, "mul") {}

DivInstruction::DivInstruction(Value *X, Value *Y, BasicBlock *Owner)
    : VisitableInstruction<DivInstruction>({X, Y}, Owner, "div") {}

CmpInstruction::CmpInstruction(Value *X, Value *Y, BasicBlock *Owner)
    : VisitableInstruction<CmpInstruction>({X, Y}, Owner, "cmp") {}

AddaInstruction::AddaInstruction(Value *X, Value *Y, BasicBlock *Owner)
    : VisitableInstruction<AddaInstruction>({X, Y}, Owner, "adda") {}

LoadInstruction::LoadInstruction(Value *Y, BasicBlock *Owner)
    : VisitableInstruction<LoadInstruction>({Y}, Owner, "load") {}

StoreInstruction::StoreInstruction(Value *Y, Value *X, BasicBlock *Owner)
    : VisitableInstruction<StoreInstruction>({Y, X}, Owner, "store") {}

MoveInstruction::MoveInstruction(Value *Y, Value *X, BasicBlock *Owner)
    : VisitableInstruction<MoveInstruction>({Y, X}, Owner, "mov") {}

PhiInstruction::PhiInstruction(const std::vector<Value *> &Values,
                               BasicBlock *Owner)
    : VisitableInstruction<PhiInstruction>(Values, Owner, "phi") {}

CallInstruction::CallInstruction(Value *X, BasicBlock *Owner)
    : VisitableInstruction<CallInstruction>({X}, Owner, "call") {}

std::vector<BasicBlock *> BasicBlockTerminator::followingBlocks() { return {}; }

RetInstruction::RetInstruction(Value *X, BasicBlock *Owner)
    : VisitableInstruction<RetInstruction>({X}, Owner, "ret") {}

EndInstruction::EndInstruction(BasicBlock *Owner)
    : VisitableInstruction<EndInstruction>({}, Owner, "end") {}

BranchInstruction::BranchInstruction(BasicBlock *Y, BasicBlock *Owner)
    : VisitableInstruction<BranchInstruction>({}, Owner, "bra"), Target(Y) {}

std::vector<BasicBlock *> BranchInstruction::followingBlocks() {
  return {Target};
}

std::vector<BasicBlock *> ConditionalBlockTerminator::followingBlocks() {
  return {Then, Else};
}

ConditionalBlockTerminator::ConditionalBlockTerminator(BasicBlock *Then,
                                                       BasicBlock *Else)
    : Then(Then), Else(Else) {}

BranchNotEqualInstruction::BranchNotEqualInstruction(CmpInstruction *Cmp,
                                                     BasicBlock *Then,
                                                     BasicBlock *Else,
                                                     BasicBlock *Owner)
    : VisitableInstruction<BranchNotEqualInstruction>({}, Owner, "bne"),
      ConditionalBlockTerminator(Then, Else) {}

BranchEqualInstruction::BranchEqualInstruction(CmpInstruction *Cmp,
                                               BasicBlock *Then,
                                               BasicBlock *Else,
                                               BasicBlock *Owner)
    : VisitableInstruction<BranchEqualInstruction>({}, Owner, "beq"),
      ConditionalBlockTerminator(Then, Else) {}

BranchLessThanEqualInstruction::BranchLessThanEqualInstruction(
    CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else, BasicBlock *Owner)
    : VisitableInstruction<BranchLessThanEqualInstruction>({}, Owner, "ble"),
      ConditionalBlockTerminator(Then, Else) {}

BranchLessThanInstruction::BranchLessThanInstruction(CmpInstruction *Cmp,
                                                     BasicBlock *Then,
                                                     BasicBlock *Else,
                                                     BasicBlock *Owner)
    : VisitableInstruction<BranchLessThanInstruction>({}, Owner, "blt"),
      ConditionalBlockTerminator(Then, Else) {}

BranchGreaterThanEqualInstruction::BranchGreaterThanEqualInstruction(
    CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else, BasicBlock *Owner)
    : VisitableInstruction<BranchGreaterThanEqualInstruction>({}, Owner, "bge"),
      ConditionalBlockTerminator(Then, Else) {}

BranchGreaterThanInstruction::BranchGreaterThanInstruction(CmpInstruction *Cmp,
                                                           BasicBlock *Then,
                                                           BasicBlock *Else,
                                                           BasicBlock *Owner)
    : VisitableInstruction<BranchGreaterThanInstruction>({}, Owner, "bgt"),
      ConditionalBlockTerminator(Then, Else) {}
