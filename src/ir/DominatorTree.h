#ifndef CS241C_IR_DOMINATORTREE_H
#define CS241C_IR_DOMINATORTREE_H

#include "BasicBlock.h"
#include <unordered_map>

namespace cs241c {

class DomNode {
public:
  DomNode(BasicBlock *BB);
  BasicBlock *BB;
  std::vector<std::unique_ptr<DomNode>> DominatedNodes;
};

class DominatorTree {
public:
  DominatorTree() {}

  // Uses algorithm proposed by Cooper, Harvey, and Kennedy from Rice
  // to build dominator tree.
  // https://www.cs.rice.edu/~keith/EMBED/dom.pdf
  std::unique_ptr<DomNode> buildDomTree(BasicBlock *Entry);

protected:
private:
  static void postOrder(std::vector<BasicBlock *> &Output, BasicBlock *Entry);
  BasicBlock * intersect(BasicBlock * Predecessor, BasicBlock * CandidateIDom);
  // Map storing immediate dominator node
  std::unordered_map<BasicBlock *, BasicBlock *> IDoms;
};

} // namespace cs241c
#endif
