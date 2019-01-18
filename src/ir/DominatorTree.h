#ifndef CS241C_IR_DOMINATORTREE_H
#define CS241C_IR_DOMINATORTREE_H

#include "BasicBlock.h"
#include <unordered_map>
#include <unordered_set>

namespace cs241c {

class DominatorTree {
public:
  DominatorTree(BasicBlock *CfgEntry);

private:
  void buildDominatorTree(BasicBlock *Entry);
  static std::vector<BasicBlock *> postOrder(BasicBlock *Entry);
  std::vector<BasicBlock *> reversePostOrder(BasicBlock *Entry);

  std::unordered_map<BasicBlock *, BasicBlock *>
  createImmediateDomMap(const std::vector<BasicBlock *> &ReversePostOrderNodes);

  std::unordered_map<BasicBlock *, uint32_t>
  createNodePositionMap(const std::vector<BasicBlock *> &ReversePostOrderNodes);

  BasicBlock *
  intersect(BasicBlock *Predecessor, BasicBlock *CandidateIDom,
            const std::unordered_map<BasicBlock *, BasicBlock *> &IDoms,
            const std::unordered_map<BasicBlock *, uint32_t> &NodePositionMap);

  std::unordered_multimap<BasicBlock *, BasicBlock *>
  iDomMapToTree(const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap);

  std::unordered_multimap<BasicBlock *, BasicBlock *> DomTree;
};

} // namespace cs241c
#endif
