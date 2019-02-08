#ifndef CS241C_IR_SSACONTEXT_H
#define CS241C_IR_SSACONTEXT_H

#include <unordered_map>

namespace cs241c {
class Value;
class Variable;

class SSAContext {
private:
  std::unordered_map<Variable *, Value *> SSAVariableMap;

public:
  const std::unordered_map<Variable *, Value *> &ssaVariableMap() const;

  void merge(const SSAContext &Source);
  void updateVariable(Variable *Arg, Value *NewVal);
  Value *lookupVariable(Variable *Arg) const;
  bool contains(Variable *Arg) const;
};
} // namespace cs241c

#endif
