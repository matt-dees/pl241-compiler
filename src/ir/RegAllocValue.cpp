#include "RegAllocValue.h"

using namespace cs241c;

uint32_t RegAllocValue::costToSpill() { return NumUses; }

void RegAllocValue::visit() { NumUses++; }