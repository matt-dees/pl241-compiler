#include <cstdint>

struct RAHeuristicInfo {
public:
  RAHeuristicInfo() : NumUses(0) {}
  uint32_t NumUses;
  int32_t spillCost() { return NumUses; }
};