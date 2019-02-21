#ifndef CS241C_REGISTERALLOCATION_H
#define CS241C_REGISTERALLOCATION_H

#include "Function.h"
#include "InterferenceGraph.h"
#include "RegAllocValue.h"
#include "Vcg.h"
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace cs241c {

class RegisterAllocator {
public:
  enum RA_REGISTER { SPILL = 0, R1 = 1, R2, R3, R4, R5, R6, R7, R8 };
  using Coloring = std::unordered_map<Value *, RA_REGISTER>;
  static const uint8_t NUM_REGISTERS = 8;

public:
  Coloring color(InterferenceGraph IG);

private:
  RegAllocValue *getValWithLowestSpillCost(InterferenceGraph &);

  void colorRecur(InterferenceGraph &IG, Coloring &CurrentColoring);

  void assignColor(InterferenceGraph &IG, Coloring &CurrentColoring,
                   RegAllocValue *NodeToColor);
  RegAllocValue *chooseNextNodeToColor(InterferenceGraph &IG);
};

class AnnotatedIG : public Vcg {
  InterferenceGraph &IG;
  const RegisterAllocator::Coloring &C;

private:
  void writeNodes(std::ofstream &OutFileStream);
  std::string lookupColor(RegAllocValue *);

public:
  AnnotatedIG(InterferenceGraph &IG, const RegisterAllocator::Coloring &C)
      : IG(IG), C(C){};

  virtual void writeGraph(std::ofstream &OutFileStream) override;
};
};     // namespace cs241c
#endif // CS241C_REGISTERALLOCATION_H
