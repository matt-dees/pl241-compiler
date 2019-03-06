#ifndef CS241C_IR_INTERFERENCEGRAPH_H
#define CS241C_IR_INTERFERENCEGRAPH_H

#include "DominatorTree.h"
#include "Function.h"
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace cs241c {
class InterferenceGraph {
public:
  struct SpillHeuristicCtx {
    static const uint32_t LOOP_MULTIPLIER = 15;
    uint32_t LoopDepth = 0;
  };
  struct IGNode {
  public:
    IGNode() = default;
    IGNode(Value *Node) : Values({Node}) {}
    IGNode(std::set<Value *> Values) : Values(Values) {}
    std::set<Value *> Values;

    void merge(IGNode *Other) {
      for (auto Val : Other->Values) {
        this->Values.insert(Val);
      }
      this->CurrentSpillCost += Other->CurrentSpillCost;
    }
    uint32_t CurrentSpillCost = 0;
    uint32_t spillCost() { return CurrentSpillCost; }
  };
  // Map node to edges
  using Graph = std::unordered_map<IGNode *, std::unordered_set<IGNode *>>;

private:
  Graph IG;
  std::vector<std::unique_ptr<IGNode>> IGNodes;

  bool interferes(IGNode *Node1, IGNode *Node2);

  std::unordered_map<Value *, IGNode *> ValueToNode;

public:
  InterferenceGraph() = default;

  Graph &graph() { return IG; }

  bool hasValue(Value *);
  bool hasNode(IGNode *Node) { return IG.find(Node) != IG.end(); }
  void addInterference(Value *From, Value *To);
  void addValue(Value *);
  void addInterferences(const std::unordered_set<Value *> &FromSet, Value *To);
  void visit(Value *, const SpillHeuristicCtx &SHC);

  std::unordered_set<IGNode *> neighbors(IGNode *Val);
  void removeNode(IGNode *Val);

  void coalesce();
}; // namespace cs241c

class IGBuilder {
public:
  IGBuilder(Function *F, DominatorTree *DT) : F(F), DT(DT){};
  void buildInterferenceGraph();
  InterferenceGraph &interferenceGraph() { return IG; }

private:
  struct IgBuildCtx {
  public:
    IgBuildCtx() : NextNode(nullptr), LiveSet({}) {}
    IgBuildCtx(BasicBlock *NextNode, std::unordered_set<Value *> LiveSet) : NextNode(NextNode), LiveSet(LiveSet) {}
    BasicBlock *NextNode;
    std::unordered_set<Value *> LiveSet;
    void merge(IgBuildCtx Other);
    IgBuildCtx &operator=(const IgBuildCtx &Other) = default;
    InterferenceGraph::SpillHeuristicCtx SHC;
  };

  InterferenceGraph IG;

  IgBuildCtx igBuild(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildIfStmt(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildLoop(IgBuildCtx CurrentCtx);
  IgBuildCtx igBuildNormal(IgBuildCtx CurrentCtx);

  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> processBlock(IgBuildCtx CurrentCtx);

  Function *F;
  DominatorTree *DT;
};
} // namespace cs241c

#endif
