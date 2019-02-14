#ifndef CS241C_REGISTERALLOCATION_H
#define CS241C_REGISTERALLOCATION_H

#include "BasicBlock.h"
#include "Vcg.h"

namespace cs241c {

class InterferenceGraph : public Vcg {
  using Graph = std::unordered_map<Value *, std::vector<Value *>>;
  Graph IG;

public:
  void addEdge(Value *From, Value *To);
  std::vector<Value *> getEdges(Value *Node);
  virtual void writeGraph();
};

class RegisterAllocation {
public:
  InterferenceGraph buildInterferenceGraph(BasicBlock::iterator);
};
};     // namespace cs241c
#endif // CS241C_REGISTERALLOCATION_H