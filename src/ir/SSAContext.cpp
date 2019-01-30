#include "SSAContext.h"

using namespace cs241c;

std::unordered_map<Variable *, Value *> &SSAContext::changes() {
  return Changes;
}

const std::unordered_map<Variable *, Value *> &
SSAContext::ssaVariableMap() const {
  return SSAVariableMap;
}

void SSAContext::updateVariable(Variable *Arg, Value *NewVal) {
  SSAVariableMap[Arg] = NewVal;
  Changes[Arg] = NewVal;
}

Value *SSAContext::lookupVariable(Variable *Arg) {
  if (!contains(Arg)) {
    return Arg;
  }
  return SSAVariableMap.at(Arg);
}

bool SSAContext::contains(Variable *Arg) {
  return SSAVariableMap.find(Arg) != SSAVariableMap.end();
}

void SSAContext::merge(const SSAContext &Source) {
  SSAVariableMap.insert(Source.ssaVariableMap().begin(),
                        Source.ssaVariableMap().end());
}

void SSAContext::clearChanges() { Changes.clear(); }
