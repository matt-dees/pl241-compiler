#include "IrGenContext.h"
#include <stdexcept>
#include <string_view>

using namespace cs241c;

std::string IrGenContext::genBasicBlockName() {
  return std::string("BB_") + std::to_string(BasicBlockCounter++);
}

int IrGenContext::genInstructionId() { return InstructionCounter++; }

void IrGenContext::beginScope() { LocalVariables.clear(); }

void IrGenContext::declare(GlobalVariable *Var) {
  if (GlobalVariables.find(Var->name()) != GlobalVariables.end()) {
    throw std::runtime_error(std::string("Redeclaring global ") + Var->name());
  }
  GlobalVariables[Var->name()] = Var;
}

void IrGenContext::declare(LocalVariable *Var) {
  if (LocalVariables.find(Var->name()) != LocalVariables.end()) {
    throw std::runtime_error(std::string("Redeclaring local ") + Var->name());
  }
  LocalVariables[Var->name()] = Var;
}

Variable *IrGenContext::lookupVariable(const std::string &Ident) {
  auto It = LocalVariables.find(Ident);
  if (It != LocalVariables.end()) {
    return It->second;
  } else {
    return GlobalVariables[Ident];
  }
}

Function *IrGenContext::lookupFuncion(const std::string &Ident) {
  auto Result = Functions.find(Ident);
  if (Result == Functions.end()) {
    throw std::runtime_error(std::string("Use of undeclared function ") +
                             Ident);
  }
  return Result->second;
}

Value *IrGenContext::makeConstant(int Val) {
  return CurrentBlock->Constants
      .emplace_back(std::make_unique<ConstantValue>(Val))
      .get();
}
