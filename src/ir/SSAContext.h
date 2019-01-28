#ifndef CS241C_SSACONTEXT_H
#define CS241C_SSACONTEXT_H

#include "Value.h"
#include "Variable.h"
#include <unordered_map>

namespace cs241c {
class SSAContext {
private:
  std::unordered_map<Variable *, Value *> SSAVariableMap;

public:
  Value *updateVariable(Variable *Arg, Value *NewVal);
  Value *lookupVariable(Variable *Arg);
};
} // namespace cs241c
#endif // CS241C_SSACONTEXT_H
