#include "Instruction.h"

using namespace cs241c;

Instruction::Instruction(std::vector<Value *> Params, std::string Name)
    : Value(""), Owner(nullptr), Params(std::move(Params)),
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

NegInstruction::NegInstruction(Value *X) : Instruction({X}, "neg") {}

AddInstruction::AddInstruction(Value *X, Value *Y)
    : Instruction({X, Y}, "add") {}

SubInstruction::SubInstruction(Value *X, Value *Y)
    : Instruction({X, Y}, "sub") {}

MulInstruction::MulInstruction(Value *X, Value *Y)
    : Instruction({X, Y}, "mul") {}

DivInstruction::DivInstruction(Value *X, Value *Y)
    : Instruction({X, Y}, "div") {}

CmpInstruction::CmpInstruction(Value *X, Value *Y)
    : Instruction({X, Y}, "cmp") {}

AddaInstruction::AddaInstruction(Value *X, Value *Y)
    : Instruction({X, Y}, "adda") {}

LoadInstruction::LoadInstruction(Value *Y) : Instruction({Y}, "load") {}

StoreInstruction::StoreInstruction(Value *Y, Value *X)
    : Instruction({Y, X}, "store") {}

MoveInstruction::MoveInstruction(Value *Y, Value *X)
    : Instruction({Y, X}, "mov") {}

PhiInstruction::PhiInstruction(const std::vector<Value *> &Values)
    : Instruction(Values, "phi") {}

CallInstruction::CallInstruction(Value *X) : Instruction({X}, "call") {}

std::vector<BasicBlock *> BasicBlockTerminator::followingBlocks() { return {}; }

RetInstruction::RetInstruction(Value *X) : Instruction({X}, "ret") {}

EndInstruction::EndInstruction() : Instruction({}, "end") {}

BranchInstruction::BranchInstruction(BasicBlock *Y)
    : Instruction({}, "bra"), Target(Y) {}

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
                                                     BasicBlock *Else)
    : Instruction({}, "bne"), ConditionalBlockTerminator(Then, Else) {}

BranchEqualInstruction::BranchEqualInstruction(CmpInstruction *Cmp,
                                               BasicBlock *Then,
                                               BasicBlock *Else)
    : Instruction({}, "beq"), ConditionalBlockTerminator(Then, Else) {}

BranchLessThanEqualInstruction::BranchLessThanEqualInstruction(
    CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : Instruction({}, "ble"), ConditionalBlockTerminator(Then, Else) {}

BranchLessThanInstruction::BranchLessThanInstruction(CmpInstruction *Cmp,
                                                     BasicBlock *Then,
                                                     BasicBlock *Else)
    : Instruction({}, "blt"), ConditionalBlockTerminator(Then, Else) {}

BranchGreaterThanEqualInstruction::BranchGreaterThanEqualInstruction(
    CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : Instruction({}, "bge"), ConditionalBlockTerminator(Then, Else) {}

BranchGreaterThanInstruction::BranchGreaterThanInstruction(CmpInstruction *Cmp,
                                                           BasicBlock *Then,
                                                           BasicBlock *Else)
    : Instruction({}, "bgt"), ConditionalBlockTerminator(Then, Else) {}
