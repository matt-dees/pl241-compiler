#ifndef CS241C_IR_DOMINATORTREE_H
#define CS241C_IR_DOMINATORTREE_H

#include "BasicBlock.h"
#include <unordered_map>

namespace cs241c {

class DominatorTree {
public:
  DominatorTree(BasicBlock *CfgEntry);

private:
  struct DomNode {
  public:
    DomNode(BasicBlock *BB) : BB(BB){};
    BasicBlock *BB;
    std::vector<std::unique_ptr<DomNode>> DominatedNodes;
  };

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

  std::unique_ptr<DomNode>
  iDomMapToTree(const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap);

  std::unique_ptr<DomNode> Head;
};

} // namespace cs241c
#endif
