#ifndef CS241C_IR_INTERFERENCEGRAPH_H
#define CS241C_IR_INTERFERENCEGRAPH_H

#include "DominatorTree.h"
#include "Function.h"
#include "RAHeuristicInfo.h"
#include <unordered_map>
#include <unordered_set>

namespace cs241c {
class InterferenceGraph {
public:
  // Map node to edges
  using Graph = std::unordered_map<Value *, std::unordered_set<Value *>>;

private:
  Graph IG;

  bool interferes(Value *Node1, Value *Node2);

  Value *getValueInGraph(Value *);
  std::unordered_map<Value *, RAHeuristicInfo> HeuristicDataMap;
  std::unordered_map<Value *, Value *> CoalesceMap;

public:
  InterferenceGraph() = default;

  std::unordered_map<Value *, Value *> &coalescedNodes() { return CoalesceMap; }
  RAHeuristicInfo &heuristicData(Value *);

  const Graph &graph() const { return IG; }

  std::unordered_set<Value *> removeNode(Value *);
  std::unordered_set<Value *> neighbors(Value *);
  bool hasNode(Value *Node);
  void addEdge(Value *From, Value *To);
  void addNode(Value *Node);
  void addEdges(const std::unordered_set<Value *> &FromSet, Value *To);

  void coalesce();
};

class IGBuilder {
public:
  IGBuilder(Function *F, DominatorTree *DT) : F(F), DT(DT){};
  void buildInterferenceGraph();
  InterferenceGraph interferenceGraph() { return IG; }

private:
  struct IgBuildCtx {
  public:
    IgBuildCtx() : NextNode(nullptr), LiveSet({}) {}
    IgBuildCtx(BasicBlock *NextNode, std::unordered_set<Value *> LiveSet) : NextNode(NextNode), LiveSet(LiveSet) {}
    BasicBlock *NextNode;
    std::unordered_set<Value *> LiveSet;
    void merge(IgBuildCtx Other);
    IgBuildCtx &operator=(const IgBuildCtx &Other) = default;
  };

  InterferenceGraph IG;

  IgBuildCtx igBuild(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildIfStmt(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildLoop(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildNormal(IgBuildCtx CurrentCtx);

  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> processBlock(BasicBlock *BB,
                                                                             std::unordered_set<Value *> LiveSet);

  Function *F;
  DominatorTree *DT;
};
} // namespace cs241c

#endif
