#ifndef CS241C_IR_DOMINATORTREE_H
#define CS241C_IR_DOMINATORTREE_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cs241c {
class BasicBlock;

class DominatorTree {
public:
  std::unordered_set<BasicBlock *> dominanceFrontier(BasicBlock *BB);
  void buildDominatorTree(BasicBlock *Entry);
  std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> DominanceFrontier;

private:
  static std::vector<BasicBlock *> postOrder(BasicBlock *Entry);
  static std::vector<BasicBlock *> reversePostOrder(BasicBlock *Entry);

  static std::unordered_map<BasicBlock *, BasicBlock *>
  createImmediateDomMap(const std::vector<BasicBlock *> &ReversePostOrderNodes);

  static std::unordered_map<BasicBlock *, uint32_t>
  createNodePositionMap(const std::vector<BasicBlock *> &ReversePostOrderNodes);

  static std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>>
  createDominanceFrontier(BasicBlock *CurrentBlock, const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap);

  static BasicBlock *intersect(BasicBlock *Predecessor, BasicBlock *CandidateIDom,
                               const std::unordered_map<BasicBlock *, BasicBlock *> &IDoms,
                               const std::unordered_map<BasicBlock *, uint32_t> &NodePositionMap);

  static std::unordered_multimap<BasicBlock *, BasicBlock *>
  iDomMapToTree(const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap);

  std::unordered_multimap<BasicBlock *, BasicBlock *> DomTree;
  std::unordered_map<BasicBlock *, BasicBlock *> IDomMap;
};

} // namespace cs241c
#endif
