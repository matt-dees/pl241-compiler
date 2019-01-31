#include "Instruction.h"
#include "assert.h"
#include <sstream>

using namespace cs241c;

Instruction::Instruction(int Id) : Id(Id) {}

BasicBlock *Instruction::getOwner() const { return Owner; }

std::string Instruction::id() const {
  return std::string("(") + std::to_string(Id) + ")";
}

std::string Instruction::toString() const {
  std::stringstream Result;
  Result << id() << ": " << getName();

  std::string_view Separator = " ";
  for (auto &Arg : getArguments()) {
    Result << Separator << Arg->id();
    Separator = ", ";
  }

  return Result.str();
}

void Instruction::setId(int NewId) { Id = NewId; }

BasicInstruction::BasicInstruction(int Id, std::vector<Value *> &&Arguments)
    : Instruction(Id), Arguments(move(Arguments)) {}

std::vector<Value *> BasicInstruction::getArguments() const {
  return Arguments;
}

void BasicInstruction::argsToSSA(cs241c::SSAContext &SSACtx) {
  std::vector<Value *> Args = getArguments();
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (auto Var = dynamic_cast<Variable *>(Args.at(i))) {
      updateArg(i, SSACtx.lookupVariable(Var));
    }
  }
}

void BasicInstruction::updateArg(unsigned long Index, Value *NewVal) {
  Arguments.at(Index) = NewVal;
}

MemoryInstruction::MemoryInstruction(int Id, Variable *Object,
                                     AddaInstruction *Address)
    : Instruction(Id), Object(Object), Address(Address) {}

BasicBlockTerminator::BasicBlockTerminator(int Id,
                                           std::vector<Value *> &&Arguments)
    : Instruction(Id), Arguments(move(Arguments)) {}

std::vector<Value *> BasicBlockTerminator::getArguments() const {
  return Arguments;
}

void BasicBlockTerminator::updateArg(unsigned long Index, Value *NewVal) {
  Arguments.at(Index) = NewVal;
}

void BasicBlockTerminator::argsToSSA(cs241c::SSAContext &SSACtx) {
  std::vector<Value *> Args = getArguments();
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (auto Var = dynamic_cast<Variable *>(Args.at(i))) {
      updateArg(i, SSACtx.lookupVariable(Var));
    }
  }
}

std::vector<BasicBlock *> BasicBlockTerminator::followingBlocks() { return {}; }

ConditionalBlockTerminator::ConditionalBlockTerminator(int Id,
                                                       CmpInstruction *Cmp,
                                                       BasicBlock *Then,
                                                       BasicBlock *Else)
    : BasicBlockTerminator(Id, {Cmp, Then, Else}) {}

std::vector<BasicBlock *> ConditionalBlockTerminator::followingBlocks() {
  return {dynamic_cast<BasicBlock *>(getArguments()[1]),
          dynamic_cast<BasicBlock *>(getArguments()[2])};
}

NegInstruction::NegInstruction(int Id, Value *X) : BasicInstruction(Id, {X}) {}
std::string_view NegInstruction::getName() const { return "neg"; }

AddInstruction::AddInstruction(int Id, Value *X, Value *Y)
    : BasicInstruction(Id, {X, Y}) {}
std::string_view AddInstruction::getName() const { return "add"; }

SubInstruction::SubInstruction(int Id, Value *X, Value *Y)
    : BasicInstruction(Id, {X, Y}) {}
std::string_view SubInstruction::getName() const { return "sub"; }

MulInstruction::MulInstruction(int Id, Value *X, Value *Y)
    : BasicInstruction(Id, {X, Y}) {}
std::string_view MulInstruction::getName() const { return "mul"; }

DivInstruction::DivInstruction(int Id, Value *X, Value *Y)
    : BasicInstruction(Id, {X, Y}) {}
std::string_view DivInstruction::getName() const { return "div"; }

CmpInstruction::CmpInstruction(int Id, Value *X, Value *Y)
    : BasicInstruction(Id, {X, Y}) {}
std::string_view CmpInstruction::getName() const { return "cmp"; }

AddaInstruction::AddaInstruction(int Id, Value *X, Value *Y)
    : BasicInstruction(Id, {X, Y}) {}
std::string_view AddaInstruction::getName() const { return "adda"; }

LoadInstruction::LoadInstruction(int Id, Variable *Object,
                                 AddaInstruction *Address)
    : MemoryInstruction(Id, Object, Address) {}

std::vector<Value *> LoadInstruction::getArguments() const { return {Address}; }

void cs241c::LoadInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 0) {
    Address = dynamic_cast<AddaInstruction *>(NewVal);
    if (!Address) {
      throw std::logic_error(
          "Paramater of LoadInstruction is not AddaInstruction.");
    }
  } else {
    throw std::range_error(
        "Index is not within the range of a LoadInstruction.");
  }
}

std::string_view LoadInstruction::getName() const { return "load"; }

StoreInstruction::StoreInstruction(int Id, Variable *Object, Value *Y,
                                   AddaInstruction *Address)
    : MemoryInstruction(Id, Object, Address), Y(Y) {}

std::vector<Value *> StoreInstruction::getArguments() const {
  return {Y, Address};
}

void StoreInstruction::updateArg(unsigned long Index, Value *NewVal) {
  switch (Index) {
  case 0:
    Y = NewVal;
    break;
  case 1:
    Address = dynamic_cast<AddaInstruction *>(NewVal);
    if (!Address) {
      throw std::logic_error(
          "Second paramater of StoreInstruction is not AddaInstruction.");
    }
    break;
  default:
    throw std::range_error(
        "Index is not within the range of a StoreInstruction.");
  }
}

std::string_view StoreInstruction::getName() const { return "store"; }

MoveInstruction::MoveInstruction(int Id, Value *Y, Value *X)
    : BasicInstruction(Id, {Y, X}), Target(X), Source(Y) {}
std::string_view MoveInstruction::getName() const { return "move"; }
void MoveInstruction::updateArgs(Value *NewTarget, Value *NewSource) {
  Target = NewTarget;
  Source = NewSource;
  Arguments = {NewSource, NewTarget};
}

PhiInstruction::PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2)
    : BasicInstruction(Id, {X1, X2}), Target(Target) {}
std::string_view PhiInstruction::getName() const { return "phi"; }

CallInstruction::CallInstruction(int Id, Function *Target,
                                 std::vector<Value *> Arguments)
    : Instruction(Id), Target(Target), Arguments(move(Arguments)) {}

std::string_view CallInstruction::getName() const { return "call"; }

std::vector<Value *> CallInstruction::getArguments() const {
  std::vector<Value *> Result;
  Result.push_back(Target);
  std::copy(Arguments.begin(), Arguments.end(), std::back_inserter(Result));
  return Result;
}

void CallInstruction::updateArg(unsigned long Index, Value *NewVal) {
  Arguments.at(Index) = NewVal;
}

RetInstruction::RetInstruction(int Id) : BasicBlockTerminator(Id, {}) {}
RetInstruction::RetInstruction(int Id, Value *X)
    : BasicBlockTerminator(Id, {X}) {}
std::string_view RetInstruction::getName() const { return "ret"; }

EndInstruction::EndInstruction(int Id) : BasicBlockTerminator(Id, {}) {}
std::string_view EndInstruction::getName() const { return "end"; }

BraInstruction::BraInstruction(int Id, BasicBlock *Y)
    : BasicBlockTerminator(Id, {Y}) {}
std::string_view BraInstruction::getName() const { return "bra"; }

std::vector<BasicBlock *> BraInstruction::followingBlocks() {
  BasicBlock *Target = dynamic_cast<BasicBlock *>(getArguments()[0]);
  assert(Target);
  return {Target};
}

BneInstruction::BneInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BneInstruction::getName() const { return "bne"; }

BeqInstruction::BeqInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BeqInstruction::getName() const { return "beq"; }

BltInstruction::BltInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BltInstruction::getName() const { return "blt"; }

BleInstruction::BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BleInstruction::getName() const { return "ble"; }

BgeInstruction::BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BgeInstruction::getName() const { return "bge"; }

BgtInstruction::BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BgtInstruction::getName() const { return "bgt"; }

ReadInstruction::ReadInstruction(int Id) : BasicInstruction(Id, {}) {}
std::string_view ReadInstruction::getName() const { return "read"; }

WriteInstruction::WriteInstruction(int Id, Value *X)
    : BasicInstruction(Id, {X}) {}
std::string_view WriteInstruction::getName() const { return "write"; }

WriteNLInstruction::WriteNLInstruction(int Id) : BasicInstruction(Id, {}) {}
std::string_view WriteNLInstruction::getName() const { return "writeNL"; }
