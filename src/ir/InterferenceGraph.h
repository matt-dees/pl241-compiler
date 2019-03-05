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
  struct IGNode {
  public:
    IGNode() = default;
    IGNode(Value *Node) : Values({Node}) {}
    IGNode(std::set<Value *> Values) : Values(Values) {}
    std::set<Value *> Values;

    std::set<Value *> &values() { return Values; }

    void merge(IGNode *Other) {
      for (auto Val : Other->Values) {
        this->Values.insert(Val);
      }
      this->NumUses += Other->NumUses;
    }
    uint32_t NumUses = 0;
    uint32_t spillCost() { return NumUses; }
  };
  // Map node to edges
  using Graph = std::unordered_map<IGNode *, std::unordered_set<IGNode *>>;

private:
  Graph IG;
  std::vector<std::unique_ptr<IGNode>> IGNodes;

  bool interferes(Value *Val1, Value *Val2);

  std::unordered_map<Value *, IGNode *> ValueToNode;

public:
  InterferenceGraph() = default;

  Graph &graph() { return IG; }

  bool hasValue(Value *);
  bool containsNodeForValue(Value *Val) { return IG.find(ValueToNode[Val]) != IG.end(); }
  void addInterference(Value *From, Value *To);
  void addValue(Value *);
  void addInterferences(const std::unordered_set<Value *> &FromSet, Value *To);
  void visit(Value *);

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
