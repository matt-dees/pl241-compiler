#ifndef CS241C_IR_INTERFERENCEGRAPH_H
#define CS241C_IR_INTERFERENCEGRAPH_H

#include "DominatorTree.h"
#include "Function.h"
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

  std::unordered_map<RegAllocValue *, std::unordered_set<RegAllocValue *>>
      WrittenEdges;

public:
  void writeEdge(std::ofstream &OutFileStream, RegAllocValue *From,
                 RegAllocValue *To);
  void reset() { WrittenEdges = {}; }
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

class IGBuilder {
public:
  IGBuilder(Function *F, DominatorTree *DT) : F(F), DT(DT){};
  void buildInterferenceGraph();
  InterferenceGraph interferenceGraph() { return IG; }

private:
  struct IgBuildCtx {
    BasicBlock *NextNode;
    std::unordered_set<RegAllocValue *> LiveSet;
    void merge(IgBuildCtx Other);
  };

  std::unordered_map<Value *, std::unique_ptr<RegAllocValue>>
      ValueToRegAllocVal;
  InterferenceGraph IG;

  IgBuildCtx igBuild(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildIfStmt(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildLoop(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildNormal(IgBuildCtx CurrentCtx);

  std::unordered_map<BasicBlock *, std::unordered_set<RegAllocValue *>>
  processBlock(BasicBlock *BB, std::unordered_set<RegAllocValue *> LiveSet);

  RegAllocValue *lookupRegAllocVal(Value *);

  Function *F;
  DominatorTree *DT;
};
}; // namespace cs241c
#endif