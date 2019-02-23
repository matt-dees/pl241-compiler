#ifndef CS241C_IR_DOMINATORTREE_H
#define CS241C_IR_DOMINATORTREE_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cs241c {
class BasicBlock;
class Function;

class DominatorTree {
  bool Reverse;
  std::unordered_multimap<BasicBlock *, BasicBlock *> DomTree;

public:
  std::unordered_map<BasicBlock *, BasicBlock *> IDomMap;
  std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> DominanceFrontier;

  DominatorTree(bool Reverse = false);
  std::unordered_set<BasicBlock *> dominanceFrontier(BasicBlock *BB);
  void buildDominatorTree(Function &F);
  bool doesBlockDominate(BasicBlock *Dominator, BasicBlock *Candidate) const;
};
} // namespace cs241c

#endif
