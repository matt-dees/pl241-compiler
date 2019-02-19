#ifndef CS241C_REGALLOCVALUE_H
#define CS241C_REGALLOCVALUE_H

#include "Value.h"

namespace cs241c {
class RegAllocValue {
  uint32_t NumUses;
  Value *Val;

public:
  RegAllocValue(Value * Val) : NumUses(0), Val(Val){};
  uint32_t costToSpill();
  void visit();
  std::string toString() { return Val->toString(); }
  Value *value() const { return Val; }
};
} // namespace cs241c
#endif // CS241C_REGALLOCVALUE_H
