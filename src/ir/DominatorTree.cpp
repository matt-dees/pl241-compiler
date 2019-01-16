#include "DominatorTree.h"
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace cs241c;
DomNode::DomNode(BasicBlock *BB) : BB(BB), DominatedNodes({}) {}

std::unique_ptr<DomNode> DominatorTree::buildDomTree(BasicBlock *Entry) {
  std::vector<BasicBlock *> Cfg;
  postOrder(Cfg, Entry);
  std::reverse(Cfg.begin(), Cfg.end());

  bool Changed = true;
  IDoms[Entry] = Entry;

  while (Changed) {
    Changed = false;
    for (auto Node : Cfg) {
      // TODO: Handle Empty Predecessor Error
      BasicBlock *CandidateIDom = Node->Predecessors.at(0);
      for (auto PredecessorIt = Node->Predecessors.begin() + 1;
           PredecessorIt != Node->Predecessors.end(); PredecessorIt++) {
        if (IDoms[*PredecessorIt] != nullptr) {
          CandidateIDom = intersect(*PredecessorIt, CandidateIDom);
        }
      }
    }
  }
}

void DominatorTree::postOrder(std::vector<BasicBlock *> &Output,
                              cs241c::BasicBlock *Entry) {
  if (Entry->Terminator->followingBlocks().empty()) {
    Output.push_back(Entry);
    return;
  }
  for (auto NextBlock : Entry->Terminator->followingBlocks()) {
    postOrder(Output, NextBlock);
  }
}

void DominatorTree::intersect(cs241c::BasicBlock *Predecessor,
                              cs241c::BasicBlock *CandidateIDom) {
  // Read page 7 of reference paper for description of intersect algorithm
  BasicBlock *Finger1;
  BasicBlock *Finger2;

  while (Finger1 != Finger2) {
    while ()
  }
}