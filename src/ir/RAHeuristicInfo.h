#ifndef CS241C_IR_RAHEURISTICSINFO_H
#define CS241C_IR_RAHEURISTICSINFO_H

#include <cstdint>

namespace cs241c {
struct RAHeuristicInfo {
public:
  RAHeuristicInfo() : NumUses(0) {}
  uint32_t NumUses;
  int32_t spillCost() { return NumUses; }
};
} // namespace cs241c

#endif
