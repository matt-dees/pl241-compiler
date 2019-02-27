#ifndef CS241C_REGISTERALLOCATION_H
#define CS241C_REGISTERALLOCATION_H

#include "Function.h"
#include "InterferenceGraph.h"
#include "Pass.h"
#include "Vcg.h"
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
  Coloring color(InterferenceGraph IG);

private:
  Value *getValWithLowestSpillCost(InterferenceGraph &);

  void color(InterferenceGraph &IG, Coloring &CurrentColoring);

  void assignColor(InterferenceGraph &IG, Coloring &CurrentColoring, Value *NodeToColor);
  Value *chooseNextNodeToColor(InterferenceGraph &IG);
};

class AnnotatedIG : public Vcg {
  InterferenceGraph *IG;
  RegisterAllocator::Coloring *C;

private:
  void writeNodes(std::ofstream &OutFileStream);
  std::string lookupColor(Value *);

public:
  AnnotatedIG(InterferenceGraph *IG, RegisterAllocator::Coloring *C) : IG(IG), C(C){};

  virtual void writeGraph(std::ofstream &OutFileStream) override;
};
} // namespace cs241c

#endif
