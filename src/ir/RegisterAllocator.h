#ifndef CS241C_REGISTERALLOCATION_H
#define CS241C_REGISTERALLOCATION_H

#include "Value.h"
#include "Vcg.h"
#include <unordered_map>
#include <unordered_set>

namespace cs241c {

class InterferenceGraph : public Vcg {
  // Map node to edges
  using Graph = std::unordered_map<Value *, std::unordered_set<Value *>>;
  Graph IG;

private:
  void writeNodes(std::ofstream &OutFileStream);
  void writeEdge(std::ofstream &OutFileStream, Value *From, Value *To);
  std::unordered_map<Value *, std::unordered_set<Value *>> WrittenEdges;

public:
  void addEdge(Value *From, Value *To);
  void addEdges(const std::unordered_set<Value *> &FromSet, Value *To);
  virtual void writeGraph(std::ofstream &OutFileStream) override;
};

};     // namespace cs241c
#endif // CS241C_REGISTERALLOCATION_H
