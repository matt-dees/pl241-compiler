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

BaseInstruction::BaseInstruction(int Id, std::vector<Value *> &&Arguments)
    : Instruction(Id), Arguments(move(Arguments)) {}

std::vector<Value *> BaseInstruction::getArguments() const { return Arguments; }

void BaseInstruction::argsToSSA(cs241c::SSAContext &SSACtx) {
  std::vector<Value *> Args = getArguments();
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (auto Var = dynamic_cast<Variable *>(Args.at(i))) {
      updateArg(i, SSACtx.lookupVariable(Var));
    }
  }
}

void BaseInstruction::updateArg(const unsigned long Index, Value *NewVal) {
  Arguments.at(Index) = NewVal;
}

BasicBlockTerminator::BasicBlockTerminator(int Id,
                                           std::vector<Value *> &&Arguments)
    : Instruction(Id), Arguments(move(Arguments)) {}

std::vector<Value *> BasicBlockTerminator::getArguments() const {
  return Arguments;
}

void BasicBlockTerminator::updateArg(const unsigned long Index, Value *NewVal) {
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

NegInstruction::NegInstruction(int Id, Value *X) : BaseInstruction(Id, {X}) {}
std::string_view NegInstruction::getName() const { return "neg"; }

AddInstruction::AddInstruction(int Id, Value *X, Value *Y)
    : BaseInstruction(Id, {X, Y}) {}
std::string_view AddInstruction::getName() const { return "add"; }

SubInstruction::SubInstruction(int Id, Value *X, Value *Y)
    : BaseInstruction(Id, {X, Y}) {}
std::string_view SubInstruction::getName() const { return "sub"; }

MulInstruction::MulInstruction(int Id, Value *X, Value *Y)
    : BaseInstruction(Id, {X, Y}) {}
std::string_view MulInstruction::getName() const { return "mul"; }

DivInstruction::DivInstruction(int Id, Value *X, Value *Y)
    : BaseInstruction(Id, {X, Y}) {}
std::string_view DivInstruction::getName() const { return "div"; }

CmpInstruction::CmpInstruction(int Id, Value *X, Value *Y)
    : BaseInstruction(Id, {X, Y}) {}
std::string_view CmpInstruction::getName() const { return "cmp"; }

AddaInstruction::AddaInstruction(int Id, Value *X, Value *Y)
    : BaseInstruction(Id, {X, Y}) {}
std::string_view AddaInstruction::getName() const { return "adda"; }

LoadInstruction::LoadInstruction(int Id, Value *Y) : BaseInstruction(Id, {Y}) {}
std::string_view LoadInstruction::getName() const { return "load"; }

StoreInstruction::StoreInstruction(int Id, Value *Y, Value *X)
    : BaseInstruction(Id, {Y, X}) {}
std::string_view StoreInstruction::getName() const { return "store"; }

MoveInstruction::MoveInstruction(int Id, Value *Y, Value *X)
    : BaseInstruction(Id, {Y, X}), Target(X), Source(Y) {}
std::string_view MoveInstruction::getName() const { return "move"; }
void MoveInstruction::updateArgs(Value *NewTarget, Value *NewSource) {
  Target = NewTarget;
  Source = NewSource;
  Arguments = {NewSource, NewTarget};
}

PhiInstruction::PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2)
    : BaseInstruction(Id, {X1, X2}), Target(Target) {}
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

void CallInstruction::updateArg(const unsigned long Index, Value *NewVal) {
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

std::string_view BltInstruction::getName() const { return "ble"; }

BleInstruction::BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BleInstruction::getName() const { return "blt"; }

BgeInstruction::BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BgeInstruction::getName() const { return "bge"; }

BgtInstruction::BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BgtInstruction::getName() const { return "bgt"; }

ReadInstruction::ReadInstruction(int Id) : BaseInstruction(Id, {}) {}
std::string_view ReadInstruction::getName() const { return "read"; }

WriteInstruction::WriteInstruction(int Id, Value *X)
    : BaseInstruction(Id, {X}) {}
std::string_view WriteInstruction::getName() const { return "write"; }

WriteNLInstruction::WriteNLInstruction(int Id) : BaseInstruction(Id, {}) {}
std::string_view WriteNLInstruction::getName() const { return "writeNL"; }
