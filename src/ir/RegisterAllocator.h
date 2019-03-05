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

public:
  Coloring color(InterferenceGraph::Graph IG);

private:
  InterferenceGraph::IGNode *getNodeWithLowestSpillCost(InterferenceGraph::Graph &IG);
  void color(InterferenceGraph::Graph &IG,
             std::unordered_map<InterferenceGraph::IGNode *, VirtualRegister> &CurrentColoring);
  void assignColor(InterferenceGraph::Graph &IG,
                   std::unordered_map<InterferenceGraph::IGNode *, VirtualRegister> &CurrentColoring,
                   InterferenceGraph::IGNode *NodeToColor);
  InterferenceGraph::IGNode *chooseNextNodeToColor(InterferenceGraph::Graph &IG);
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
