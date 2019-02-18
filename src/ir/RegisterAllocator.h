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
  void addEdge(RegAllocValue *From, RegAllocValue *To);
  void addEdges(const std::unordered_set<RegAllocValue *> &FromSet,
                RegAllocValue *To);
  virtual void writeGraph(std::ofstream &OutFileStream) override;
};

class RegisterAllocator {
public:
  std::unordered_map<Value *, int8_t> color(const InterferenceGraph &IG);
};
};     // namespace cs241c
#endif // CS241C_REGISTERALLOCATION_H
