#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"
#include "SSAContext.h"
#include "Variable.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>

using namespace cs241c;

Instruction::Instruction(int Id, std::vector<Value *> &&Arguments)
    : Id(Id), Arguments(move(Arguments)) {}

BasicBlock *Instruction::getOwner() const { return Owner; }

std::string Instruction::name() const {
  return std::string("(") + std::to_string(Id) + ")";
}

std::string Instruction::toString() const {
  std::stringstream Result;
  Result << name() << ": " << mnemonic();

  std::string_view Separator = " ";
  for (auto &Arg : arguments()) {
    Result << Separator << Arg->name();
    Separator = ", ";
  }

  return Result.str();
}

void Instruction::setId(int Id) { this->Id = Id; }

std::vector<Value *> &Instruction::arguments() { return Arguments; }
const std::vector<Value *> &Instruction::arguments() const { return Arguments; }

void Instruction::argsToSSA(SSAContext &SSACtx) {
  std::vector<Value *> Args = arguments();
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (auto Var = dynamic_cast<Variable *>(Args.at(i))) {
      updateArg(i, SSACtx.lookupVariable(Var));
    }
  }
}

void Instruction::updateArg(unsigned long Index, Value *NewVal) {
  Arguments.at(Index) = NewVal;
}

MemoryInstruction::MemoryInstruction(int Id, Variable *Object,
                                     std::vector<Value *> &&Arguments)
    : Instruction(Id, move(Arguments)), Object(Object) {}

Variable *MemoryInstruction::object() const { return Object; }

BasicBlockTerminator::BasicBlockTerminator(int Id,
                                           std::vector<Value *> &&Arguments)
    : Instruction(Id, move(Arguments)) {}

void BasicBlockTerminator::argsToSSA(SSAContext &SSACtx) {
  std::vector<Value *> Args = arguments();
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
  return {dynamic_cast<BasicBlock *>(arguments()[1]),
          dynamic_cast<BasicBlock *>(arguments()[2])};
}

NegInstruction::NegInstruction(int Id, Value *X) : Instruction(Id, {X}) {}
std::string_view NegInstruction::mnemonic() const { return "neg"; }

AddInstruction::AddInstruction(int Id, Value *X, Value *Y)
    : Instruction(Id, {X, Y}) {}
std::string_view AddInstruction::mnemonic() const { return "add"; }

SubInstruction::SubInstruction(int Id, Value *X, Value *Y)
    : Instruction(Id, {X, Y}) {}
std::string_view SubInstruction::mnemonic() const { return "sub"; }

MulInstruction::MulInstruction(int Id, Value *X, Value *Y)
    : Instruction(Id, {X, Y}) {}
std::string_view MulInstruction::mnemonic() const { return "mul"; }

DivInstruction::DivInstruction(int Id, Value *X, Value *Y)
    : Instruction(Id, {X, Y}) {}
std::string_view DivInstruction::mnemonic() const { return "div"; }

CmpInstruction::CmpInstruction(int Id, Value *X, Value *Y)
    : Instruction(Id, {X, Y}) {}
std::string_view CmpInstruction::mnemonic() const { return "cmp"; }

AddaInstruction::AddaInstruction(int Id, Value *X, Value *Y)
    : Instruction(Id, {X, Y}) {}
std::string_view AddaInstruction::mnemonic() const { return "adda"; }

LoadInstruction::LoadInstruction(int Id, Variable *Object,
                                 AddaInstruction *Address)
    : MemoryInstruction(Id, Object, {Address}) {}

void LoadInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 0 && !dynamic_cast<AddaInstruction *>(NewVal)) {
    throw std::logic_error(
        "Paramater of LoadInstruction is not AddaInstruction.");
  }
  MemoryInstruction::updateArg(Index, NewVal);
}

std::string_view LoadInstruction::mnemonic() const { return "load"; }

StoreInstruction::StoreInstruction(int Id, Variable *Object, Value *Y,
                                   AddaInstruction *Address)
    : MemoryInstruction(Id, Object, {Y, Address}) {}

void StoreInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 1 && !dynamic_cast<AddaInstruction *>(NewVal)) {
    throw std::logic_error(
        "Second paramater of StoreInstruction is not AddaInstruction.");
  }
  MemoryInstruction::updateArg(Index, NewVal);
}

std::string_view StoreInstruction::mnemonic() const { return "store"; }

MoveInstruction::MoveInstruction(int Id, Value *Y, Value *X)
    : Instruction(Id, {Y, X}), Target(X), Source(Y) {}
std::string_view MoveInstruction::mnemonic() const { return "move"; }

void MoveInstruction::updateArgs(Value *NewTarget, Value *NewSource) {
  Target = NewTarget;
  Source = NewSource;
  arguments() = {NewSource, NewTarget};
}

PhiInstruction::PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2)
    : Instruction(Id, {X1, X2}), Target(Target) {}
std::string_view PhiInstruction::mnemonic() const { return "phi"; }

namespace {
std::vector<Value *>
prepareCallArguments(Function *Target, const std::vector<Value *> &Arguments) {
  std::vector<Value *> CallArguments;
  CallArguments.reserve(Arguments.size() + 1);
  CallArguments.push_back(Target);
  std::copy(Arguments.begin(), Arguments.end(),
            std::back_inserter(CallArguments));
  return CallArguments;
}
} // namespace

CallInstruction::CallInstruction(int Id, Function *Target,
                                 std::vector<Value *> Arguments)
    : Instruction(Id, prepareCallArguments(Target, Arguments)) {}

std::string_view CallInstruction::mnemonic() const { return "call"; }

RetInstruction::RetInstruction(int Id) : BasicBlockTerminator(Id, {}) {}
RetInstruction::RetInstruction(int Id, Value *X)
    : BasicBlockTerminator(Id, {X}) {}
std::string_view RetInstruction::mnemonic() const { return "ret"; }

EndInstruction::EndInstruction(int Id) : BasicBlockTerminator(Id, {}) {}
std::string_view EndInstruction::mnemonic() const { return "end"; }

BraInstruction::BraInstruction(int Id, BasicBlock *Y)
    : BasicBlockTerminator(Id, {Y}) {}
std::string_view BraInstruction::mnemonic() const { return "bra"; }

std::vector<BasicBlock *> BraInstruction::followingBlocks() {
  auto *Target = dynamic_cast<BasicBlock *>(arguments()[0]);
  assert(Target);
  return {Target};
}

BneInstruction::BneInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BneInstruction::mnemonic() const { return "bne"; }

BeqInstruction::BeqInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BeqInstruction::mnemonic() const { return "beq"; }

BltInstruction::BltInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BltInstruction::mnemonic() const { return "blt"; }

BleInstruction::BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BleInstruction::mnemonic() const { return "ble"; }

BgeInstruction::BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BgeInstruction::mnemonic() const { return "bge"; }

BgtInstruction::BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then,
                               BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

std::string_view BgtInstruction::mnemonic() const { return "bgt"; }

ReadInstruction::ReadInstruction(int Id) : Instruction(Id, {}) {}
std::string_view ReadInstruction::mnemonic() const { return "read"; }

WriteInstruction::WriteInstruction(int Id, Value *X) : Instruction(Id, {X}) {}
std::string_view WriteInstruction::mnemonic() const { return "write"; }

WriteNLInstruction::WriteNLInstruction(int Id) : Instruction(Id, {}) {}
std::string_view WriteNLInstruction::mnemonic() const { return "writeNL"; }
