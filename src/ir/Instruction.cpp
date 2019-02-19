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

Instruction::Instruction(InstructionType InstrT, int Id, Value *Arg1) : Instruction(InstrT, Id, Arg1, nullptr) {}

Instruction::Instruction(InstructionType InstrT, int Id, Value *Arg1, Value *Arg2)
    : Value(valTy(InstrT)), InstrT(InstrT), Id(Id), Arg1(Arg1), Arg2(Arg2) {}

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

vector<Value *> Instruction::arguments() const {
  vector<Value *> Arguments;
  if (Arg1 != nullptr) {
    Arguments.push_back(Arg1);
    if (Arg2 != nullptr) {
      Arguments.push_back(Arg2);
    }
  }
  return Arguments;
}

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

void Instruction::updateArg(int Index, Value *NewVal) {
  Value *&Arg = Index == 0 ? Arg1 : Arg2;
  Arg = NewVal;
}

MemoryInstruction::MemoryInstruction(InstructionType InstrT, int Id, Variable *Object, Value *Arg1)
    : MemoryInstruction(InstrT, Id, Object, Arg1, nullptr) {}

MemoryInstruction::MemoryInstruction(InstructionType InstrT, int Id, Variable *Object, Value *Arg1, Value *Arg2)
    : Instruction(InstrT, Id, Arg1, Arg2), Object(Object) {}

Variable *MemoryInstruction::object() const { return Object; }

BasicBlockTerminator::BasicBlockTerminator(InstructionType InstrT, int Id)
    : BasicBlockTerminator(InstrT, Id, nullptr, nullptr) {}

BasicBlockTerminator::BasicBlockTerminator(InstructionType InstrT, int Id, Value *Arg1)
    : BasicBlockTerminator(InstrT, Id, Arg1, nullptr) {}

BasicBlockTerminator::BasicBlockTerminator(InstructionType InstrT, int Id, Value *Arg1, Value *Arg2)
    : Instruction(InstrT, Id, Arg1, Arg2) {}

BasicBlock *BasicBlockTerminator::target() { return nullptr; }

ConditionalBlockTerminator::ConditionalBlockTerminator(InstructionType InstrT, int Id, CmpInstruction *Cmp,
                                                       BasicBlock *Target)
    : BasicBlockTerminator(InstrT, Id, Cmp, Target) {}

BasicBlock *ConditionalBlockTerminator::target() { return dynamic_cast<BasicBlock *>(arguments()[1]); }

CmpInstruction::CmpInstruction(int Id, Value *X, Value *Y) : Instruction(T::Cmp, Id, X, Y) {}

AddaInstruction::AddaInstruction(int Id, Value *X, Value *Y) : Instruction(T::Adda, Id, X, Y) {}

LoadInstruction::LoadInstruction(int Id, Variable *Object, AddaInstruction *Address)
    : MemoryInstruction(T::Load, Id, Object, Address) {}

StoreInstruction::StoreInstruction(int Id, Variable *Object, Value *Y, AddaInstruction *Address)
    : MemoryInstruction(T::Store, Id, Object, Y, Address) {}

MoveInstruction::MoveInstruction(int Id, Value *Y, Value *X) : Instruction(T::Move, Id, Y, X) {}

void MoveInstruction::updateArgs(Value *NewTarget, Value *NewSource) { arguments() = {NewSource, NewTarget}; }

Value *MoveInstruction::source() const { return arguments()[0]; }

Value *MoveInstruction::target() const { return arguments()[1]; }

PhiInstruction::PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2)
    : Instruction(T::Phi, Id, X1, X2), Target(Target) {}

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

BasicBlock *BraInstruction::target() {
  auto *Target = dynamic_cast<BasicBlock *>(arguments()[0]);
  assert(Target);
  return Target;
}
