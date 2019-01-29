#include "IrGenContext.h"
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

void IrGenContext::genPhiInstructions() {
  for (auto &F : CompilationUnit->functions()) {
    for (auto &BB : F->basicBlocks()) {
      insertPhiInstructions(BB.get());
    }
  }
}

void IrGenContext::insertPhiInstructions(cs241c::BasicBlock *BB) {
  for (auto I : *BB) {
    if (auto MI = dynamic_cast<MoveInstruction *>(I)) {
      if (auto Var = dynamic_cast<Variable *>(MI->Target)) {
        for (auto DFEntry : CompilationUnit->DT.dominanceFrontier(BB)) {
          DFEntry->insertPhiInstruction(Var, MI->Source, genInstructionId(),
                                        BB);
        }
      }
    }
  }
}

void IrGenContext::compUnitToSSA() {
  genPhiInstructions();
  CompilationUnit->toSSA();
}