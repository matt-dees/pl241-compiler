#include "IrGenContext.h"
#include <stdexcept>
#include <string_view>

using namespace cs241c;

Value *IrGenContext::makeConstant(int Val) {
  return CurrentBlock->Constants
      .emplace_back(std::make_unique<ConstantValue>(Val))
      .get();
}

void IrGenContext::declareGlobal(GlobalVariable *Var) {
  if (GlobalVariables.find(Var->toString()) != GlobalVariables.end()) {
    throw std::runtime_error(std::string("Redeclaring global ") +
                             Var->toString());
  }
  GlobalVariables[Var->toString()] = Var;
}

void IrGenContext::clearLocalScope() { LocalVariables.clear(); }

Value *IrGenContext::lookupVariable(const std::string &Ident) {
  auto It = LocalVariables.find(Ident);
  if (It != LocalVariables.end()) {
    return It->second.get();
  } else {
    return GlobalVariables[Ident];
  }
}

Function *IrGenContext::lookupFuncion(const std::string &Ident) {
  return Functions[Ident];
}
