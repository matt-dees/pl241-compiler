#include "Instruction.h"

using namespace cs241c;

Instruction::Instruction(std::vector<Value *> &&Params)
    : Params(std::move(Params)) {}

BasicBlock *Instruction::getOwner() const { return Owner; }

std::string Instruction::toString() const {
  std::string Result{getName()};

  std::string_view Separator = " ";
  for (auto &Param : Params) {
    Result.append(Separator);
    Result.append(Param->toString());
    Separator = ", ";
  }

  return Result;
}

template <typename T>
BaseInstruction<T>::BaseInstruction(std::vector<Value *> &&Params)
    : Instruction(move(Params)) {}

template <typename T> void BaseInstruction<T>::visit(InstructionVisitor *V) {
  V->visit(static_cast<T *>(this));
}

BasicBlockTerminator::BasicBlockTerminator(std::vector<Value *> &&Params)
    : Instruction(move(Params)) {}

std::vector<BasicBlock *> BasicBlockTerminator::followingBlocks() { return {}; }

template <typename T>
BaseBasicBlockTerminator<T>::BaseBasicBlockTerminator(
    std::vector<Value *> &&Params)
    : BasicBlockTerminator(move(Params)) {}

template <typename T>
void BaseBasicBlockTerminator<T>::visit(InstructionVisitor *V) {
  V->visit(static_cast<T *>(this));
}

ConditionalBlockTerminator::ConditionalBlockTerminator(CmpInstruction *Cmp,
                                                       BasicBlock *Then,
                                                       BasicBlock *Else)
    : BasicBlockTerminator({Cmp, Then, Else}) {}

std::vector<BasicBlock *> ConditionalBlockTerminator::followingBlocks() {
  return {};
}

template <typename T>
BaseConditionalBlockTerminator<T>::BaseConditionalBlockTerminator(
    CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Cmp, Then, Else) {}

template <typename T>
void BaseConditionalBlockTerminator<T>::visit(InstructionVisitor *V) {
  V->visit(static_cast<T *>(this));
}

NegInstruction::NegInstruction(Value *X) : BaseInstruction({X}) {}
std::string_view NegInstruction::getName() const { return "neg"; }

AddInstruction::AddInstruction(Value *X, Value *Y) : BaseInstruction({X, Y}) {}
std::string_view AddInstruction::getName() const { return "add"; }

SubInstruction::SubInstruction(Value *X, Value *Y) : BaseInstruction({X, Y}) {}
std::string_view SubInstruction::getName() const { return "sub"; }

MulInstruction::MulInstruction(Value *X, Value *Y) : BaseInstruction({X, Y}) {}
std::string_view MulInstruction::getName() const { return "mul"; }

DivInstruction::DivInstruction(Value *X, Value *Y) : BaseInstruction({X, Y}) {}
std::string_view DivInstruction::getName() const { return "div"; }

CmpInstruction::CmpInstruction(Value *X, Value *Y) : BaseInstruction({X, Y}) {}
std::string_view CmpInstruction::getName() const { return "cmp"; }

AddaInstruction::AddaInstruction(Value *X, Value *Y)
    : BaseInstruction({X, Y}) {}
std::string_view AddaInstruction::getName() const { return "adda"; }

LoadInstruction::LoadInstruction(Value *Y) : BaseInstruction({Y}) {}
std::string_view LoadInstruction::getName() const { return "load"; }

StoreInstruction::StoreInstruction(Value *Y, Value *X)
    : BaseInstruction({Y, X}) {}
std::string_view StoreInstruction::getName() const { return "store"; }

MoveInstruction::MoveInstruction(Value *Y, Value *X)
    : BaseInstruction({Y, X}) {}
std::string_view MoveInstruction::getName() const { return "move"; }

PhiInstruction::PhiInstruction(Value *X1, Value *X2)
    : BaseInstruction({X1, X2}) {}
std::string_view PhiInstruction::getName() const { return "phi"; }

CallInstruction::CallInstruction(Value *X) : BaseInstruction({X}) {}
std::string_view CallInstruction::getName() const { return "call"; }

RetInstruction::RetInstruction(Value *X) : BaseBasicBlockTerminator({X}) {}
std::string_view RetInstruction::getName() const { return "ret"; }

EndInstruction::EndInstruction() : BaseBasicBlockTerminator({}) {}
std::string_view EndInstruction::getName() const { return "end"; }

BraInstruction::BraInstruction(BasicBlock *Y)
    : BaseBasicBlockTerminator(std::vector<Value *>{Y}) {}
std::string_view BraInstruction::getName() const { return "bra"; }

std::vector<BasicBlock *> BraInstruction::followingBlocks() { return {}; }

BneInstruction::BneInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : BaseConditionalBlockTerminator(Cmp, Then, Else) {}

std::string_view BneInstruction::getName() const { return "bne"; }

BeqInstruction::BeqInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : BaseConditionalBlockTerminator(Cmp, Then, Else) {}

std::string_view BeqInstruction::getName() const { return "beq"; }

BltInstruction::BltInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : BaseConditionalBlockTerminator(Cmp, Then, Else) {}

std::string_view BltInstruction::getName() const { return "ble"; }

BleInstruction::BleInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : BaseConditionalBlockTerminator(Cmp, Then, Else) {}

std::string_view BleInstruction::getName() const { return "blt"; }

BgeInstruction::BgeInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : BaseConditionalBlockTerminator(Cmp, Then, Else) {}

std::string_view BgeInstruction::getName() const { return "bge"; }

BgtInstruction::BgtInstruction(CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : BaseConditionalBlockTerminator(Cmp, Then, Else) {}

std::string_view BgtInstruction::getName() const { return "bgt"; }
