#include "IrGenContext.h"
#include "Module.h"
#include <algorithm>
#include <stdexcept>
#include <string_view>

using namespace cs241c;

IrGenContext::IrGenContext(Module *CompilationUnit)
    : CompilationUnit(CompilationUnit) {}

Value *IrGenContext::globalBase() { return CompilationUnit->globalBase(); }

BasicBlock *&IrGenContext::currentBlock() { return CurrentBlock; }

std::vector<std::unique_ptr<ConstantValue>> &&IrGenContext::constants() {
  return move(Constants);
}

std::vector<std::unique_ptr<BasicBlock>> &&IrGenContext::blocks() {
  return move(Blocks);
}

std::string IrGenContext::genBasicBlockName() {
  return std::string("BB_") + std::to_string(BasicBlockCounter++);
}

int IrGenContext::genInstructionId() { return InstructionCounter++; }

void IrGenContext::beginScope() {
  Constants.clear();
  LocalsTable.clear();
  Blocks.clear();
}

void IrGenContext::declare(std::unique_ptr<Function> Func) {
  if (FunctionTable.find(Func->name()) != FunctionTable.end()) {
    throw std::runtime_error(std::string("Redeclaring function ") +
                             Func->name());
  }
  FunctionTable[Func->name()] = Func.get();
  CompilationUnit->functions().push_back(move(Func));
}

void IrGenContext::declare(Symbol Sym, std::unique_ptr<GlobalVariable> Var) {
  if (GlobalsTable.find(Var->name()) != GlobalsTable.end()) {
    throw std::runtime_error(std::string("Redeclaring global variable ") +
                             Var->name());
  }
  GlobalsTable[Var->name()] = Sym;
  CompilationUnit->globals().push_back(move(Var));
}

void IrGenContext::declare(Symbol Sym) {
  const std::string &VarName = Sym.Var->name();
  if (LocalsTable.find(VarName) != LocalsTable.end()) {
    throw std::runtime_error(std::string("Redeclaring local variable ") +
                             VarName);
  }
  LocalsTable[VarName] = Sym;
}

Symbol IrGenContext::lookupVariable(const std::string &Ident) {
  auto Local = LocalsTable.find(Ident);
  if (Local != LocalsTable.end()) {
    return Local->second;
  }
  auto Global = GlobalsTable.find(Ident);
  if (Global != GlobalsTable.end()) {
    return Global->second;
  }
  throw std::runtime_error(std::string("Use of undeclared variable ") + Ident);
}

Function *IrGenContext::lookupFuncion(const std::string &Ident) {
  auto Result = FunctionTable.find(Ident);
  if (Result == FunctionTable.end()) {
    throw std::runtime_error(std::string("Use of undeclared function ") +
                             Ident);
  }
  return Result->second;
}

ConstantValue *IrGenContext::makeConstant(int Val) {
  return Constants.emplace_back(std::make_unique<ConstantValue>(Val)).get();
}

BasicBlock *IrGenContext::makeBasicBlock() {
  return Blocks.emplace_back(std::make_unique<BasicBlock>(genBasicBlockName()))
      .get();
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
  static std::unordered_set<BasicBlock *> Visited = {};
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
  static std::unordered_set<BasicBlock *> Visited;
  if (Visited.find(CurrentBB) != Visited.end()) {
    return;
  }
  auto Phis = CurrentBB->genPhis();
  Visited.insert(CurrentBB);
  for (auto DFEntry : CompilationUnit->DT.dominanceFrontier(CurrentBB)) {
    for (auto &Phi : Phis) {
      Phi->setId(genInstructionId());
      DFEntry->insertPhiInstruction(std::move(Phi));
      Visited.erase(DFEntry);
      genAllPhiInstructions(DFEntry);
    }
  }
  for (auto BB : CurrentBB->terminator()->followingBlocks()) {
    genAllPhiInstructions(BB);
  }
}

void IrGenContext::propagateChangeToPhis(cs241c::BasicBlock *SourceBB,
                                         cs241c::Variable *ChangedVar,
                                         cs241c::Value *NewVal) {
  for (auto BB : CompilationUnit->DT.dominanceFrontier(SourceBB)) {
    if (std::find(BB->predecessors().begin(), BB->predecessors().end(),
                  SourceBB) != BB->predecessors().end()) {
      BB->updatePhiInst(SourceBB, ChangedVar, NewVal);
    }
  }
}