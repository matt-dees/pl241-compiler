#include "IrGenContext.h"
#include "Module.h"
#include "NameGen.h"
#include <algorithm>
#include <stdexcept>
#include <string_view>

using namespace cs241c;
using namespace std;

IrGenContext::IrGenContext(Module *CompilationUnit) : CompilationUnit(CompilationUnit) {}

Value *IrGenContext::globalBase() { return CompilationUnit->globalBase(); }

BasicBlock *&IrGenContext::currentBlock() { return CurrentBlock; }

Variable *&IrGenContext::currentVariable() { return CurrentVariable; }

const unordered_map<string, Symbol> &IrGenContext::localsTable() const { return LocalsTable; }

string IrGenContext::genBasicBlockName() { return NameGen::genBasicBlockName(); }

int IrGenContext::genInstructionId() { return NameGen::genInstructionId(); }

void IrGenContext::beginScope() { LocalsTable.clear(); }

void IrGenContext::declare(unique_ptr<Function> Func) {
  if (FunctionTable.find(Func->name()) != FunctionTable.end()) {
    throw runtime_error(string("Redeclaring function ") + Func->name());
  }
  FunctionTable[Func->name()] = Func.get();
  CurrentFunction = Func.get();
  CompilationUnit->functions().push_back(move(Func));
}

void IrGenContext::declare(Symbol Sym, unique_ptr<GlobalVariable> Var) {
  if (GlobalsTable.find(Var->ident()) != GlobalsTable.end()) {
    throw runtime_error(string("Redeclaring global variable ") + Var->ident());
  }
  GlobalsTable[Var->ident()] = Sym;
  CompilationUnit->globals().push_back(move(Var));
}

void IrGenContext::declare(Symbol Sym) {
  const string &VarName = Sym.Var->ident();
  if (LocalsTable.find(VarName) != LocalsTable.end()) {
    throw runtime_error(string("Redeclaring local variable ") + VarName);
  }
  LocalsTable[VarName] = Sym;
}

Symbol IrGenContext::lookupVariable(const string &Ident) {
  auto Local = LocalsTable.find(Ident);
  if (Local != LocalsTable.end()) {
    return Local->second;
  }
  auto Global = GlobalsTable.find(Ident);
  if (Global != GlobalsTable.end()) {
    return Global->second;
  }
  throw runtime_error(string("Use of undeclared variable ") + Ident);
}

Function *IrGenContext::lookupFuncion(const string &Ident) {
  auto Result = FunctionTable.find(Ident);
  if (Result == FunctionTable.end()) {
    throw runtime_error(string("Use of undeclared function ") + Ident);
  }
  return Result->second;
}

ConstantValue *IrGenContext::makeConstant(int Val) {
  for (auto &CV : CurrentFunction->constants()) {
    if (CV->Val == Val) {
      return CV.get();
    }
  }
  return CurrentFunction->constants().emplace_back(make_unique<ConstantValue>(Val)).get();
}

Instruction *IrGenContext::makeInstruction(InstructionType InstrT) { return makeInstruction(InstrT, nullptr, nullptr); }

Instruction *IrGenContext::makeInstruction(InstructionType InstrT, Value *Arg0) {
  return makeInstruction(InstrT, Arg0, nullptr);
}

Instruction *IrGenContext::makeInstruction(InstructionType InstrT, Value *Arg0, Value *Arg1) {
  auto Instr = std::make_unique<Instruction>(InstrT, genInstructionId(), Arg0, Arg1);
  if (!DisableStorageAssignment && isSubtype(Instr->ValTy, ValueType::Value)) {
    Instr->storage() = CurrentVariable;
  }
  Instruction *InstrP = Instr.get();
  CurrentBlock->appendInstruction(move(Instr));
  return InstrP;
}

BasicBlock *IrGenContext::makeBasicBlock() {
  return CurrentFunction->basicBlocks().emplace_back(make_unique<BasicBlock>(genBasicBlockName())).get();
}
