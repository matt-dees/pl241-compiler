#ifndef CS241C_REGISTERALLOCATION_H
#define CS241C_REGISTERALLOCATION_H

#include "RegAllocValue.h"
#include "Vcg.h"
#include <unordered_map>
#include <unordered_set>

namespace cs241c {

class InterferenceGraph : public Vcg {
  // Map node to edges
  using Graph =
      std::unordered_map<RegAllocValue *, std::unordered_set<RegAllocValue *>>;
  Graph IG;

private:
  void writeNodes(std::ofstream &OutFileStream);
  void writeEdge(std::ofstream &OutFileStream, RegAllocValue *From,
                 RegAllocValue *To);
  std::unordered_map<RegAllocValue *, std::unordered_set<RegAllocValue *>>
      WrittenEdges;

public:
  void addNode(RegAllocValue *Node);
  std::unordered_set<RegAllocValue *> removeNode(RegAllocValue *);
  std::unordered_set<RegAllocValue *> neighbors(RegAllocValue *);
  bool hasNode(RegAllocValue *Node);
  void addEdge(RegAllocValue *From, RegAllocValue *To);
  void addEdges(const std::unordered_set<RegAllocValue *> &FromSet,
                RegAllocValue *To);
  virtual void writeGraph(std::ofstream &OutFileStream) override;
  const Graph &graph() const { return IG; }
};

class RegisterAllocator {
public:
  enum RA_REGISTER { SPILL = 0, R1 = 1, R2, R3, R4, R5, R6, R7, R8 };
  static const int8_t NUM_REGISTERS = 8;
  using Coloring = std::unordered_map<Value *, RA_REGISTER>;

public:
  Coloring color(InterferenceGraph IG);

private:
  RegAllocValue *getValWithLowestSpillCost(InterferenceGraph &);

  void colorRecur(InterferenceGraph &IG, Coloring &CurrentColoring);

  void assignColor(InterferenceGraph &IG, Coloring &CurrentColoring,
                   RegAllocValue *NodeToColor);
  RegAllocValue *chooseNextNodeToColor(InterferenceGraph &IG);
};
};     // namespace cs241c
#endif // CS241C_REGISTERALLOCATION_H
