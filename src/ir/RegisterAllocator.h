#ifndef CS241C_REGISTERALLOCATION_H
#define CS241C_REGISTERALLOCATION_H

#include "Function.h"
#include "InterferenceGraph.h"
#include "Pass.h"
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace cs241c {
class RegisterAllocator {
public:
  typedef uint32_t VirtualRegister;
  using Coloring = std::unordered_map<Value *, VirtualRegister>;
  static const uint8_t NUM_REGISTERS = 8;

private:
  VirtualRegister FreeStackSlotBegin = NUM_REGISTERS + 1;

public:
  Coloring color(InterferenceGraph IG, Function &);

private:
  Value *getValWithLowestSpillCost(InterferenceGraph &);
  void color(InterferenceGraph &IG, Coloring &CurrentColoring);
  void assignColor(InterferenceGraph &IG, Coloring &CurrentColoring, Value *NodeToColor);
  Value *chooseNextNodeToColor(InterferenceGraph &IG);
};

class AnnotatedIG {
  InterferenceGraph *IG;
  RegisterAllocator::Coloring *C;

public:
  std::string lookupColor(Value *);
  InterferenceGraph &interferenceGraph() const { return *IG; }
  AnnotatedIG(InterferenceGraph *IG, RegisterAllocator::Coloring *C) : IG(IG), C(C){};
};
} // namespace cs241c

#endif
