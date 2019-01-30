#ifndef CS241C_IR_SSACONTEXT_H
#define CS241C_IR_SSACONTEXT_H

#include "Value.h"
#include "Variable.h"
#include <unordered_map>

namespace cs241c {
class SSAContext {
private:
  std::unordered_map<Variable *, Value *> SSAVariableMap;
  std::unordered_map<Variable *, Value *> Changes;

public:
  std::unordered_map<Variable *, Value *> &changes();
  const std::unordered_map<Variable *, Value *> &ssaVariableMap() const;

  void merge(const SSAContext &Source);
  void updateVariable(Variable *Arg, Value *NewVal);
  Value *lookupVariable(Variable *Arg);
  bool contains(Variable *Arg);
  void clearChanges();
};
} // namespace cs241c

#endif
