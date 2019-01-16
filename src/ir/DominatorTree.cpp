#include "DominatorTree.h"
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace cs241c;
DomNode::DomNode(BasicBlock *BB) : BB(BB), DominatedNodes({}) {}

std::unique_ptr<DomNode> DominatorTree::buildDomTree(BasicBlock *Entry) {
  fillReversePostOrderCfg(Entry);
  fillNodePositionMap();
  fillIDomMap(Entry);
}

void DominatorTree::fillReversePostOrderCfg(BasicBlock *Entry) {
  postOrder(ReversePostOrderCfg, Entry);
  std::reverse(ReversePostOrderCfg.begin(), ReversePostOrderCfg.end());
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

BasicBlock *DominatorTree::intersect(
    BasicBlock *Predecessor, BasicBlock *CandidateIDom,
    std::unordered_map<BasicBlock *, uint32_t> &NodePositionMap) {
  // Read page 7 of reference paper for description of intersect algorithm
  BasicBlock *Finger1 = Predecessor;
  BasicBlock *Finger2 = CandidateIDom;

  while (Finger1 != Finger2) {
    while (NodePositionMap[Finger1] < NodePositionMap[Finger2]) {
      Finger1 = IDoms[Finger1];
    }
    while (NodePositionMap[Finger2] < NodePositionMap[Finger1]) {
      Finger2 = IDoms[Finger2];
    }
  }
  return Finger1;
}

void DominatorTree::fillNodePositionMap() {
  for (uint32_t i = 0; i < ReversePostOrderCfg.size(); ++i) {
    NodePositionMap[ReversePostOrderCfg.at(i)] = i;
  }
}

void DominatorTree::fillIDomMap(BasicBlock *Entry) {
  bool Changed = true;
  IDoms[Entry] = Entry;

  while (Changed) {
    Changed = false;
    for (auto Node : ReversePostOrderCfg) {
      if (Node->Predecessors.empty()) {
        // Unreachable Node or start node. Will not include in dominator tree.
        continue;
      }
      BasicBlock *CandidateIDom = Node->Predecessors.at(0);
      for (auto PredecessorIt = Node->Predecessors.begin() + 1;
           PredecessorIt != Node->Predecessors.end(); PredecessorIt++) {
        if (IDoms.find(*PredecessorIt) != IDoms.end()) {
          CandidateIDom =
              intersect(*PredecessorIt, CandidateIDom, NodePositionMap);
        }
      }
      if (IDoms[Node] != CandidateIDom) {
        IDoms[Node] = CandidateIDom;
        Changed = true;
      }
    }
  }
}