#include "IrGenContext.h"
#include <stdexcept>
#include <string_view>

using namespace cs241c;

BasicBlock *&IrGenContext::currentBlock() { return CurrentBlock; }

std::vector<std::unique_ptr<ConstantValue>> &IrGenContext::constants() {
  return Constants;
}

std::string IrGenContext::genBasicBlockName() {
  return std::string("BB_") + std::to_string(BasicBlockCounter++);
}

int IrGenContext::genInstructionId() { return InstructionCounter++; }

void IrGenContext::beginScope() {
  Constants.clear();
  LocalVariables.clear();
}

void IrGenContext::declare(Function *Func) {
  if (Functions.find(Func->name()) != Functions.end()) {
    throw std::runtime_error(std::string("Redeclaring function ") +
                             Func->name());
  }
  Functions[Func->name()] = Func;
}

void IrGenContext::declare(GlobalVariable *Var) {
  if (GlobalVariables.find(Var->name()) != GlobalVariables.end()) {
    throw std::runtime_error(std::string("Redeclaring global variable ") +
                             Var->name());
  }
  GlobalVariables[Var->name()] = Var;
}

void IrGenContext::declare(LocalVariable *Var) {
  if (LocalVariables.find(Var->name()) != LocalVariables.end()) {
    throw std::runtime_error(std::string("Redeclaring local variable ") +
                             Var->name());
  }
  LocalVariables[Var->name()] = Var;
}

Variable *IrGenContext::lookupVariable(const std::string &Ident) {
  auto Local = LocalVariables.find(Ident);
  if (Local != LocalVariables.end()) {
    return Local->second;
  }
  auto Global = GlobalVariables.find(Ident);
  if (Global != GlobalVariables.end()) {
    return Global->second;
  }
  throw std::runtime_error(std::string("Use of undeclared variable ") + Ident);
}

Function *IrGenContext::lookupFuncion(const std::string &Ident) {
  auto Result = Functions.find(Ident);
  if (Result == Functions.end()) {
    throw std::runtime_error(std::string("Use of undeclared function ") +
                             Ident);
  }
  return Result->second;
}

ConstantValue *IrGenContext::makeConstant(int Val) {
  return Constants.emplace_back(std::make_unique<ConstantValue>(Val)).get();
}
