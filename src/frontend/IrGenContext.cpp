#include "IrGenContext.h"

using namespace cs241c;

Value *IrGenContext::makeConstant(int Val) {
  return Values.emplace_back(std::make_unique<ConstantValue>(Val)).get();
}

Value *IrGenContext::lookupVariable(const std::string &Ident) {
  auto It = LocalVariables.find(Ident);
  if (It != LocalVariables.end()) {
    return It->second.get();
  } else {
    return GlobalVariables[Ident].get();
  }
}
