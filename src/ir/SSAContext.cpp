#include "SSAContext.h"
#include "Variable.h"

using namespace cs241c;
using namespace std;

const unordered_map<Variable *, Value *> &SSAContext::ssaVariableMap() const {
  return SSAVariableMap;
}

void SSAContext::updateVariable(Variable *Arg, Value *NewVal) {
  if (auto VarVal = dynamic_cast<Variable *>(NewVal)) {
    NewVal = SSAVariableMap.at(VarVal);
  }
  SSAVariableMap[Arg] = NewVal;
}

Value *SSAContext::lookupVariable(Variable *Arg) const {
  if (!contains(Arg)) {
    return Arg;
  }
  return SSAVariableMap.at(Arg);
}

bool SSAContext::contains(Variable *Arg) const {
  return SSAVariableMap.find(Arg) != SSAVariableMap.end();
}

void SSAContext::merge(const SSAContext &Source) {
  SSAVariableMap.insert(Source.ssaVariableMap().begin(),
                        Source.ssaVariableMap().end());
}
