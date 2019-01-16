#include "DominatorTree.h"
#include <stack>
#include <unordered_set>

using namespace cs241c;
DomNode::DomNode(BasicBlock *BB) : BB(BB), DominatedNodes({}) {}

std::unique_ptr<DomNode> DominatorTree::buildDomTree(BasicBlock *Entry) {}

void DominatorTree::postOrder(std::vector<BasicBlock *> *Output,
                              cs241c::BasicBlock *Entry) {
  if (Entry->Terminator->followingBlocks().empty()) {
    Output->push_back(Entry);
    return;
  }
  for (auto NextBlock : Entry->Terminator->followingBlocks()) {
    postOrder(Output, NextBlock);
  }
}