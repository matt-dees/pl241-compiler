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

Instruction::Instruction(int Id, vector<Value *> &&Arguments) : Id(Id), Arguments(move(Arguments)) {}

bool Instruction::operator==(const Instruction &other) const {
  return (typeid(this).hash_code() == typeid(other).hash_code()) && other.arguments() == this->arguments();
}

BasicBlock *Instruction::getOwner() const { return Owner; }

string Instruction::name() const { return string("(") + to_string(Id) + ")"; }

bool Instruction::isPreLive() const { return false; }

string Instruction::toString() const {
  stringstream Result;
  Result << name() << ": " << mnemonic();

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

bool Instruction::updateArgs(const std::unordered_map<Value *, Value *> &UpdateCtx) {
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

MemoryInstruction::MemoryInstruction(int Id, Variable *Object, vector<Value *> &&Arguments)
    : Instruction(Id, move(Arguments)), Object(Object) {}

Variable *MemoryInstruction::object() const { return Object; }

BasicBlockTerminator::BasicBlockTerminator(int Id, vector<Value *> &&Arguments) : Instruction(Id, move(Arguments)) {}

vector<BasicBlock *> BasicBlockTerminator::followingBlocks() { return {}; }

ConditionalBlockTerminator::ConditionalBlockTerminator(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : BasicBlockTerminator(Id, {Cmp, Then, Else}) {}

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

NegInstruction::NegInstruction(int Id, Value *X) : Instruction(Id, {X}) {}
string_view NegInstruction::mnemonic() const { return "neg"; }

AddInstruction::AddInstruction(int Id, Value *X, Value *Y) : Instruction(Id, {X, Y}) {}
string_view AddInstruction::mnemonic() const { return "add"; }

SubInstruction::SubInstruction(int Id, Value *X, Value *Y) : Instruction(Id, {X, Y}) {}
string_view SubInstruction::mnemonic() const { return "sub"; }

MulInstruction::MulInstruction(int Id, Value *X, Value *Y) : Instruction(Id, {X, Y}) {}
string_view MulInstruction::mnemonic() const { return "mul"; }

DivInstruction::DivInstruction(int Id, Value *X, Value *Y) : Instruction(Id, {X, Y}) {}
string_view DivInstruction::mnemonic() const { return "div"; }

CmpInstruction::CmpInstruction(int Id, Value *X, Value *Y) : Instruction(Id, {X, Y}) {}
string_view CmpInstruction::mnemonic() const { return "cmp"; }

AddaInstruction::AddaInstruction(int Id, Value *X, Value *Y) : Instruction(Id, {X, Y}) {}
string_view AddaInstruction::mnemonic() const { return "adda"; }

LoadInstruction::LoadInstruction(int Id, Variable *Object, AddaInstruction *Address)
    : MemoryInstruction(Id, Object, {Address}) {}

void LoadInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 0 && !dynamic_cast<AddaInstruction *>(NewVal)) {
    throw logic_error("Paramater of LoadInstruction is not AddaInstruction.");
  }
  MemoryInstruction::updateArg(Index, NewVal);
}

string_view LoadInstruction::mnemonic() const { return "load"; }

StoreInstruction::StoreInstruction(int Id, Variable *Object, Value *Y, AddaInstruction *Address)
    : MemoryInstruction(Id, Object, {Y, Address}) {}

void StoreInstruction::updateArg(unsigned long Index, Value *NewVal) {
  if (Index == 1 && !dynamic_cast<AddaInstruction *>(NewVal)) {
    throw logic_error("Second paramater of StoreInstruction is not AddaInstruction.");
  }
  MemoryInstruction::updateArg(Index, NewVal);
}

string_view StoreInstruction::mnemonic() const { return "store"; }

bool StoreInstruction::isPreLive() const { return true; }

MoveInstruction::MoveInstruction(int Id, Value *Y, Value *X) : Instruction(Id, {Y, X}) {}
string_view MoveInstruction::mnemonic() const { return "move"; }

void MoveInstruction::updateArgs(Value *NewTarget, Value *NewSource) { arguments() = {NewSource, NewTarget}; }

bool MoveInstruction::isPreLive() const { return true; }

Value *MoveInstruction::source() const { return arguments()[0]; }

Value *MoveInstruction::target() const { return arguments()[1]; }

PhiInstruction::PhiInstruction(int Id, Variable *Target, Value *X1, Value *X2)
    : Instruction(Id, {X1, X2}), Target(Target) {}
string_view PhiInstruction::mnemonic() const { return "phi"; }

namespace {
vector<Value *> prepareCallArguments(Function *Target, const vector<Value *> &Arguments) {
  vector<Value *> CallArguments;
  CallArguments.reserve(Arguments.size() + 1);
  CallArguments.push_back(Target);
  copy(Arguments.begin(), Arguments.end(), back_inserter(CallArguments));
  return CallArguments;
}
} // namespace

CallInstruction::CallInstruction(int Id, Function *Target, vector<Value *> Arguments)
    : Instruction(Id, prepareCallArguments(Target, Arguments)) {}

string_view CallInstruction::mnemonic() const { return "call"; }

bool CallInstruction::isPreLive() const { return true; }

RetInstruction::RetInstruction(int Id) : BasicBlockTerminator(Id, {}) {}
RetInstruction::RetInstruction(int Id, Value *X) : BasicBlockTerminator(Id, {X}) {}
string_view RetInstruction::mnemonic() const { return "ret"; }
bool RetInstruction::isPreLive() const { return true; }

EndInstruction::EndInstruction(int Id) : BasicBlockTerminator(Id, {}) {}
string_view EndInstruction::mnemonic() const { return "end"; }
bool EndInstruction::isPreLive() const { return true; }

BraInstruction::BraInstruction(int Id, BasicBlock *Y) : BasicBlockTerminator(Id, {Y}) {}
string_view BraInstruction::mnemonic() const { return "bra"; }

vector<BasicBlock *> BraInstruction::followingBlocks() {
  auto *Target = dynamic_cast<BasicBlock *>(arguments()[0]);
  assert(Target);
  return {Target};
}

BneInstruction::BneInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

string_view BneInstruction::mnemonic() const { return "bne"; }

BeqInstruction::BeqInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

string_view BeqInstruction::mnemonic() const { return "beq"; }

BltInstruction::BltInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

string_view BltInstruction::mnemonic() const { return "blt"; }

BleInstruction::BleInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

string_view BleInstruction::mnemonic() const { return "ble"; }

BgeInstruction::BgeInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

string_view BgeInstruction::mnemonic() const { return "bge"; }

BgtInstruction::BgtInstruction(int Id, CmpInstruction *Cmp, BasicBlock *Then, BasicBlock *Else)
    : ConditionalBlockTerminator(Id, Cmp, Then, Else) {}

string_view BgtInstruction::mnemonic() const { return "bgt"; }

ReadInstruction::ReadInstruction(int Id) : Instruction(Id, {}) {}
string_view ReadInstruction::mnemonic() const { return "read"; }

bool ReadInstruction::isPreLive() const { return true; }

WriteInstruction::WriteInstruction(int Id, Value *X) : Instruction(Id, {X}) {}
string_view WriteInstruction::mnemonic() const { return "write"; }

bool WriteInstruction::isPreLive() const { return true; }

WriteNLInstruction::WriteNLInstruction(int Id) : Instruction(Id, {}) {}
string_view WriteNLInstruction::mnemonic() const { return "writeNL"; }

bool WriteNLInstruction::isPreLive() const { return true; }
