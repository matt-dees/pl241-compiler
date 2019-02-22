#ifndef CS241C_IR_INTERFERENCEGRAPH_H
#define CS241C_IR_INTERFERENCEGRAPH_H

#include "DominatorTree.h"
#include "Function.h"
#include "RAHeuristicInfo.h"
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
  std::unordered_map<Value *, RAHeuristicInfo> HeuristicDataMap;
  std::unordered_map<Value *, std::unordered_set<Value *>> WrittenEdges;

public:
  InterferenceGraph() = default;

  RAHeuristicInfo &heuristicData(Value *);
  void writeEdge(std::ofstream &OutFileStream, Value *From, Value *To);
  void reset() { WrittenEdges = {}; }
  void addNode(Value *Node);
  std::unordered_set<Value *> removeNode(Value *);
  std::unordered_set<Value *> neighbors(Value *);
  bool hasNode(Value *Node);
  void addEdge(Value *From, Value *To);
  void addEdges(const std::unordered_set<Value *> &FromSet, Value *To);
  virtual void writeGraph(std::ofstream &OutFileStream) override;
  const Graph &graph() const { return IG; }
};

class IGBuilder {
public:
  IGBuilder(Function *F, DominatorTree *DT) : F(F), DT(DT){};
  void buildInterferenceGraph();
  InterferenceGraph interferenceGraph() { return IG; }

private:
  struct IgBuildCtx {
  public:
    BasicBlock *NextNode;
    std::unordered_set<Value *> LiveSet;
    void merge(IgBuildCtx Other);
    IgBuildCtx &operator=(const IgBuildCtx &Other) {
      NextNode = Other.NextNode;
      LiveSet = Other.LiveSet;
      return *this;
    }
  };

  InterferenceGraph IG;

  IgBuildCtx igBuild(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildIfStmt(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildLoop(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildNormal(IgBuildCtx CurrentCtx);

  std::unordered_map<BasicBlock *, std::unordered_set<Value *>>
  processBlock(BasicBlock *BB, std::unordered_set<Value *> LiveSet);

  Function *F;
  DominatorTree *DT;
};
}; // namespace cs241c
#endif