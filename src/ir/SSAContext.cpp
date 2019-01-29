#include "SSAContext.h"

using namespace cs241c;

void SSAContext::updateVariable(Variable *Arg, Value *NewVal) {
  SSAVariableMap[Arg] = NewVal;
}

Value *SSAContext::lookupVariable(Variable *Arg) {
  return SSAVariableMap.at(Arg);
}

void SSAContext::merge(const SSAContext &Source) {
  SSAVariableMap.insert(Source.getMap().begin(), Source.getMap().end());
}

std::unordered_map<Variable *, Value *> SSAContext::getMap() const {
  return SSAVariableMap;
}