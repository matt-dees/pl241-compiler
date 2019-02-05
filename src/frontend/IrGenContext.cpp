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
  return CurrentFunction->constants().emplace_back(make_unique<ConstantValue>(Val)).get();
}

BasicBlock *IrGenContext::makeBasicBlock() {
  return CurrentFunction->basicBlocks().emplace_back(make_unique<BasicBlock>(genBasicBlockName())).get();
}

void IrGenContext::compUnitToSSA() {
  for (auto &F : CompilationUnit->Functions) {
    // Note: Assumes entry is the first basic block
    BasicBlock *Entry = F->basicBlocks().at(0).get();
    SSAContext SSACtx;
    genAllPhiInstructions(Entry);
    nodeToSSA(Entry, SSACtx);
  }
}

SSAContext IrGenContext::nodeToSSA(BasicBlock *CurrentBB, SSAContext SSACtx) {
  static unordered_set<BasicBlock *> Visited = {};
  if (Visited.find(CurrentBB) != Visited.end()) {
    // Already visited this node. Skip.
    return SSACtx;
  }

  CurrentBB->toSSA(SSACtx);
  Visited.insert(CurrentBB);

  for (auto VarChange : SSACtx.ssaVariableMap()) {
    propagateChangeToPhis(CurrentBB, VarChange.first, VarChange.second);
  }
  for (auto BB : CurrentBB->terminator()->followingBlocks()) {
    SSAContext Ret = nodeToSSA(BB, SSACtx);
    SSACtx.merge(Ret);
  }
  return SSACtx;
}

void IrGenContext::genAllPhiInstructions(BasicBlock *CurrentBB) {
  static unordered_set<BasicBlock *> Visited;
  if (Visited.find(CurrentBB) != Visited.end()) {
    return;
  }
  auto Phis = CurrentBB->genPhis();
  Visited.insert(CurrentBB);
  for (auto DFEntry : CompilationUnit->DT.dominanceFrontier(CurrentBB)) {
    for (auto &Phi : Phis) {
      Phi->setId(genInstructionId());
      DFEntry->insertPhiInstruction(move(Phi));
      Visited.erase(DFEntry);
      genAllPhiInstructions(DFEntry);
    }
  }
  for (auto BB : CurrentBB->terminator()->followingBlocks()) {
    genAllPhiInstructions(BB);
  }
}

void IrGenContext::propagateChangeToPhis(cs241c::BasicBlock *SourceBB, cs241c::Variable *ChangedVar,
                                         cs241c::Value *NewVal) {
  for (auto BB : CompilationUnit->DT.dominanceFrontier(SourceBB)) {
    if (find(BB->predecessors().begin(), BB->predecessors().end(), SourceBB) != BB->predecessors().end()) {
      BB->updatePhiInst(SourceBB, ChangedVar, NewVal);
    }
  }
}
