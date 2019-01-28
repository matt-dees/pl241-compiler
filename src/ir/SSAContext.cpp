#include "SSAContext.h"

using namespace cs241c;

Value *SSAContext::updateVariable(Variable *Arg, Value *NewVal) {
  if (SSAVariableMap.find(Arg) != SSAVariableMap.end()) {
    return SSAVariableMap.at(Arg);
  }
  if (auto ValVar = dynamic_cast<Variable *>(NewVal)) {
    NewVal = SSAVariableMap.at(ValVar);
  }
  SSAVariableMap.at(Arg) = NewVal;
  return Arg;
}

Value *SSAContext::lookupVariable(Variable *Arg) {
  return SSAVariableMap.at(Arg);
}