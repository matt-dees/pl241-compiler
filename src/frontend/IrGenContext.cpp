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

void IrGenContext::declare(std::unique_ptr<GlobalVariable> Var) {
  if (GlobalsTable.find(Var->name()) != GlobalsTable.end()) {
    throw std::runtime_error(std::string("Redeclaring global variable ") +
                             Var->name());
  }
  GlobalsTable[Var->name()] = Var.get();
  CompilationUnit->globals().push_back(move(Var));
}

void IrGenContext::declare(LocalVariable *Var) {
  if (LocalsTable.find(Var->name()) != LocalsTable.end()) {
    throw std::runtime_error(std::string("Redeclaring local variable ") +
                             Var->name());
  }
  LocalsTable[Var->name()] = Var;
}

Variable *IrGenContext::lookupVariable(const std::string &Ident) {
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
