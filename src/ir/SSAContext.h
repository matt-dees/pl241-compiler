#ifndef CS241C_SSACONTEXT_H
#define CS241C_SSACONTEXT_H

#include "Value.h"
#include "Variable.h"
#include <unordered_map>

namespace cs241c {
class SSAContext {
private:
  std::unordered_map<Variable *, Value *> SSAVariableMap;
  std::unordered_map<Variable *, Value *> Changes;

public:
  std::unordered_map<Variable *, Value *> getMap() const;
  void merge(const SSAContext &Source);
  void updateVariable(Variable *Arg, Value *NewVal);
  Value *lookupVariable(Variable *Arg);
  bool contains(Variable *Arg);
  void clearChanges();
  std::unordered_map<Variable *, Value *> changes() { return Changes; }
};
} // namespace cs241c
#endif // CS241C_SSACONTEXT_H
