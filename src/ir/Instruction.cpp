#include "Instruction.h"
#include "BasicBlock.h"
#include "SSAContext.h"
#include "Variable.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

using namespace cs241c;
using namespace std;

using T = InstructionType;

ValueRef::ValueRef() : ValTy(ValueType::Undef) {}
ValueRef::ValueRef(Value *Ptr) : ValTy(Ptr ? Ptr->ValTy : ValueType::Undef), Ptr(Ptr) {}
ValueRef::ValueRef(ValueType Ty, int Id) : ValTy(Ty), Id(Id) {}
bool ValueRef::isUndef() { return ValTy == ValueType::Undef; }
ValueRef::operator Value *() const { return Ptr; }
Value *ValueRef::operator->() const { return Ptr; }

bool ValueRef::operator==(ValueRef Other) const {
  if (ValTy == Other.ValTy) {
    if (ValTy == ValueType::Register) {
      return Id == Other.Id;
    }
    return Ptr == Other.Ptr;
  }
  return false;
}
bool ValueRef::operator<(ValueRef Other) const {
  if (ValTy == Other.ValTy) {
    if (ValTy == ValueType::Register) {
      return Id < Other.Id;
    }
    return Ptr < Other.Ptr;
  }
  return ValTy < Other.ValTy;
}

Instruction::Instruction(InstructionType InstrT, int Id) : Instruction(InstrT, Id, nullptr, nullptr) {}

Instruction::Instruction(InstructionType InstrT, int Id, ValueRef Arg1) : Instruction(InstrT, Id, Arg1, nullptr) {}

Instruction::Instruction(InstructionType InstrT, int Id, ValueRef Arg1, ValueRef Arg2)
    : Value(valTy(InstrT)), InstrT(InstrT), Id(Id), Args{Arg1, Arg2} {
  checkArgs();
}

bool Instruction::operator==(const Instruction &other) const {
  return (typeid(this).hash_code() == typeid(other).hash_code()) && other.arguments() == this->arguments();
}

BasicBlock *Instruction::owner() const { return Owner; }

BasicBlock *&Instruction::owner() { return Owner; }

Variable *&Instruction::storage() { return Storage; }

string Instruction::name() const {
  stringstream S;
  if (Storage != nullptr) {
    S << Storage->ident();
  }
  S << "(" << Id << ")";
  return S.str();
}

string Instruction::toString() const {
  stringstream Result;
  Result << name() << ": " << mnemonic(InstrT);

  string_view Separator = " ";
  for (auto Arg : Args) {
    if (Arg.ValTy == ValueType::Undef)
      break;

    Result << Separator;

    if (Arg.ValTy == ValueType::Register)
      Result << "R" << Arg.Id;
    else if (Arg.ValTy == ValueType::StackSlot)
      Result << "S" << Arg.Id;
    else
      Result << Arg->name();

    Separator = ", ";
  }

  return Result.str();
}

void Instruction::setId(int Id) { this->Id = Id; }

vector<ValueRef> Instruction::arguments() const {
  vector<ValueRef> Arguments;
  if (Args[0].ValTy != ValueType::Undef) {
    Arguments.push_back(Args[0]);
    if (Args[1].ValTy != ValueType::Undef) {
      Arguments.push_back(Args[1]);
    }
  }
  return Arguments;
}

BasicBlock *Instruction::target() {
  Value *Target = nullptr;
  if (InstrT == T::Bra) {
    Target = Args[0];
  } else if (isConditionalBranch(InstrT)) {
    Target = Args[1];
  } else {
    return nullptr;
  }
  return dynamic_cast<BasicBlock *>(Target);
}

Variable *&Instruction::object() { return Object; }

bool Instruction::updateArgs(const map<ValueRef, ValueRef> &UpdateCtx) {
  vector<ValueRef> Args = arguments();
  bool DidChange = false;
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (UpdateCtx.find(Args[i]) != UpdateCtx.end()) {
      updateArg(i, UpdateCtx.at(Args[i]));
      DidChange = true;
    }
  }
  checkArgs();
  return DidChange;
}

bool Instruction::updateArgs(const SSAContext &SSAVarCtx) {
  vector<ValueRef> Args = arguments();
  bool DidChange = false;
  for (long unsigned int i = 0; i < Args.size(); ++i) {
    if (auto Var = dynamic_cast<Variable *>(&*Args[i])) {
      updateArg(i, SSAVarCtx.lookupVariable(Var));
      DidChange = true;
    }
  }
  checkArgs();
  return DidChange;
}

void Instruction::updateArg(int Index, ValueRef NewVal) { Args[Index] = NewVal; }

void Instruction::checkArgs() {
  const InstructionSignature &Sig = signature(InstrT);
  for (size_t I = 0; I < Args.size(); ++I) {
    auto Arg = Args[I];
    ValueType Type = Sig.Args[I];

    if (Arg.isUndef()) {
      if (Type != ValueType::Undef && Type != ValueType::Any) {
        // The explicit any part is for ret instructions... maybe not very
        // clean.
        stringstream M;
        M << "Argument " << I << " of instruction " << Id << " (" << mnemonic(InstrT) << ") is missing, expected "
          << cs241c::name(Type) << ".";
        throw logic_error(M.str());
      }
    } else if (!isSubtype(Arg.ValTy, Type)) {
      stringstream M;
      M << "Argument " << I << " of instruction " << Id << " (" << mnemonic(InstrT) << ") has type "
        << cs241c::name(Arg.ValTy) << ", expected " << cs241c::name(Type) << ".";
      throw logic_error(M.str());
    }
  }
}
