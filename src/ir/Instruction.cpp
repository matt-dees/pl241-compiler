#include "Instruction.h"
#include "BasicBlock.h"
#include "Function.h"
#include "SSAContext.h"
#include "Variable.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <typeinfo>

using namespace cs241c;
using namespace std;

using T = InstructionType;

Instruction::Instruction(InstructionType InstrT, int Id, vector<Value *> &&Arguments)
    : InstrT(InstrT), Id(Id), Arguments(move(Arguments)) {}

bool Instruction::operator==(const Instruction &other) const {
  return (typeid(this).hash_code() == typeid(other).hash_code()) && other.arguments() == this->arguments();
}

BasicBlock *Instruction::getOwner() const { return Owner; }

string Instruction::name() const { return string("(") + to_string(Id) + ")"; }

string Instruction::toString() const {
  stringstream Result;
  Result << name() << ": " << mnemonic(InstrT);

  string_view Separator = " ";
  for (auto &Arg : arguments()) {
    Result << Separator << Arg->name();
    Separator = ", ";
  }

  return Result.str();
}

void Instruction::setId(int Id) { this->Id = Id; }

vector<Value *> &Instruction::arguments() { return Arguments; }
const vector<Value *> &Instruction::arguments() const { return Arguments; }

bool Instruction::updateArgs(const unordered_map<Value *, Value *> &UpdateCtx) {
  vector<Value *> Args = arguments();
  bool DidChange = false;
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (UpdateCtx.find(Args[i]) != UpdateCtx.end()) {
      updateArg(i, UpdateCtx.at(Args[i]));
      DidChange = true;
    }
  }
  return DidChange;
}

bool Instruction::updateArgs(const SSAContext &SSAVarCtx) {
  vector<Value *> Args = arguments();
  bool DidChange = false;
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (auto Var = dynamic_cast<Variable *>(Args[i])) {
      updateArg(i, SSAVarCtx.lookupVariable(Var));
      DidChange = true;
    }
  }
  return DidChange;
}

void Instruction::updateArg(unsigned long Index, Value *NewVal) { Arguments.at(Index) = NewVal; }

MemoryInstruction::MemoryInstruction(InstructionType InstrT, int Id, Variable *Object, vector<Value *> &&Arguments)
    : Instruction(InstrT, Id, move(Arguments)), Object(Object) {}

Variable *MemoryInstruction::object() const { return Object; }

BasicBlockTerminator::BasicBlockTerminator(InstructionType InstrT, int Id, vector<Value *> &&Arguments)
    : Instruction(InstrT, Id, move(Arguments)) {}

vector<BasicBlock *> BasicBlockTerminator::followingBlocks() { return {}; }

ConditionalBlockTerminator::ConditionalBlockTerminator(InstructionType InstrT, int Id, CmpInstruction *Cmp,
                                                       BasicBlock *Then, BasicBlock *Else)
    : BasicBlockTerminator(InstrT, Id, {Cmp, Then, Else}) {}

BasicBlock *ConditionalBlockTerminator::elseBlock() const { return dynamic_cast<BasicBlock *>(arguments()[2]); }

vector<BasicBlock *> ConditionalBlockTerminator::followingBlocks() {
  return {dynamic_cast<BasicBlock *>(arguments()[1]), dynamic_cast<BasicBlock *>(arguments()[2])};
}

void ConditionalBlockTerminator::updateTarget(BasicBlock *OldTarget, BasicBlock *NewTarget) {
  auto &Arguments = arguments();
  auto OldTargetIt = find(Arguments.begin(), Arguments.end(), OldTarget);
  *OldTargetIt = NewTarget;
  auto &OldPred = OldTarget->predecessors();
  OldPred.erase(remove(OldPred.begin(), OldPred.end(), getOwner()), OldPred.end());
  NewTarget->predecessors().push_back(getOwner());
}

CmpInstruction::CmpInstruction(int Id, Value *X, Value *Y) : Instruction(T::Cmp, Id, {X, Y}) {}

AddaInstruction::AddaInstruction(int Id, Value *X, Value *Y) : Instruction(T::Adda, Id, {X, Y}) {}

LoadInstruction::LoadInstruction(int Id, Variable *Object, AddaInstruction *Address)
    : MemoryInstruction(T::Load, Id, Object, {Address}) {}

void LoadInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 0 && !dynamic_cast<AddaInstruction *>(NewVal)) {
    throw logic_error("Paramater of LoadInstruction is not AddaInstruction.");
  }
  MemoryInstruction::updateArg(Index, NewVal);
}

StoreInstruction::StoreInstruction(int Id, Variable *Object, Value *Y, AddaInstruction *Address)
    : MemoryInstruction(T::Store, Id, Object, {Y, Address}) {}

void StoreInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 1 && !dynamic_cast<AddaInstruction *>(NewVal)) {
    throw logic_error("Second paramater of StoreInstruction is not AddaInstruction.");
  }
  MemoryInstruction::updateArg(Index, NewVal);
}

MoveInstruction::MoveInstruction(int Id, Value *Y, Value *X) : Instruction(T::Move, Id, {Y, X}) {}

void MoveInstruction::updateArgs(Value *NewTarget, Value *NewSource) { arguments() = {NewSource, NewTarget}; }

Value *MoveInstruction::source() const { return arguments()[0]; }

Value *MoveInstruction::target() const { return arguments()[1]; }

PhiInstruction::PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2)
    : Instruction(T::Phi, Id, {X1, X2}), Target(Target) {}

namespace {
vector<Value *> prepareCallArguments(Function *Target, const vector<Value *> &Arguments) {
  vector<Value *> CallArguments;
  CallArguments.reserve(Arguments.size() + 1);
  CallArguments.push_back(Target);
  copy(Arguments.begin(), Arguments.end(), back_inserter(CallArguments));
  return CallArguments;
}
} // namespace

BraInstruction::BraInstruction(int Id, BasicBlock *Y) : BasicBlockTerminator(T::Bra, Id, {Y}) {}

vector<BasicBlock *> BraInstruction::followingBlocks() {
  auto *Target = dynamic_cast<BasicBlock *>(arguments()[0]);
  assert(Target);
  return {Target};
}

void BraInstruction::updateTarget(BasicBlock *NewTarget) {
  auto *OldTarget = dynamic_cast<BasicBlock *>(arguments()[0]);
  auto &OldPred = OldTarget->predecessors();
  OldPred.erase(remove(OldPred.begin(), OldPred.end(), getOwner()), OldPred.end());
  arguments()[0] = NewTarget;
  NewTarget->predecessors().push_back(getOwner());
}
