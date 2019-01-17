#include "DominatorTree.h"
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace cs241c;

DominatorTree::DominatorTree(cs241c::BasicBlock *CfgEntry) {
  buildDominatorTree(CfgEntry);
}

void DominatorTree::buildDominatorTree(BasicBlock *Entry) {
  std::vector<BasicBlock *> ReversePostOrderCfg = reversePostOrder(Entry);

  std::unordered_map<BasicBlock *, BasicBlock *> IDoms =
      createImmediateDomMap(ReversePostOrderCfg);

  Head = move(iDomMapToTree(IDoms));
}

std::unique_ptr<DominatorTree::DomNode> DominatorTree::iDomMapToTree(
    const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
  std::unordered_set<BasicBlock *> Visited = {};
  for (auto BB : IDomMap) {
    if (!Visited.contains())
  }
}

std::vector<BasicBlock *> DominatorTree::reversePostOrder(BasicBlock *Entry) {
  std::vector<BasicBlock *> Cfg = postOrder(Entry);
  std::reverse(Cfg.begin(), Cfg.end());
  return Cfg;
}

std::vector<BasicBlock *> DominatorTree::postOrder(BasicBlock *Entry) {
  static std::vector<BasicBlock *> PostOrderNodes;

  if (Entry->Terminator->followingBlocks().empty()) {
    PostOrderNodes.push_back(Entry);
    return PostOrderNodes;
  }

  for (auto NextBlock : Entry->Terminator->followingBlocks()) {
    postOrder(NextBlock);
  }
  PostOrderNodes.push_back(Entry);
  return PostOrderNodes;
}

std::unordered_map<BasicBlock *, BasicBlock *>
DominatorTree::createImmediateDomMap(
    const std::vector<BasicBlock *> &ReversePostOrderNodes) {
  std::unordered_map<BasicBlock *, BasicBlock *> IDoms;
  std::unordered_map<BasicBlock *, uint32_t> NodePositionMap =
      createNodePositionMap(ReversePostOrderNodes);

  bool Changed = true;
  IDoms[ReversePostOrderNodes.back()] = ReversePostOrderNodes.back();

  while (Changed) {
    Changed = false;
    for (auto Node : ReversePostOrderNodes) {
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
  return IDoms;
}

BasicBlock *DominatorTree::intersect(
    BasicBlock *Predecessor, BasicBlock *CandidateIDom,
    const std::unordered_map<BasicBlock *, BasicBlock *> &IDoms,
    const std::unordered_map<BasicBlock *, uint32_t> &NodePositionMap) {
  // Read page 7 of reference paper for description of intersect algorithm
  BasicBlock *Finger1 = Predecessor;
  BasicBlock *Finger2 = CandidateIDom;

  while (Finger1 != Finger2) {
    while (NodePositionMap.at(Finger1) < NodePositionMap.at(Finger2)) {
      Finger1 = IDoms.at(Finger1);
    }
    while (NodePositionMap.at(Finger2) < NodePositionMap.at(Finger1)) {
      Finger2 = IDoms.at(Finger2);
    }
  }
  return Finger1;
}

std::unordered_map<BasicBlock *, uint32_t> DominatorTree::createNodePositionMap(
    const std::vector<BasicBlock *> &ReversePostOrderNodes) {
  std::unordered_map<BasicBlock *, uint32_t> NodePositionMap;
  for (uint32_t i = 0; i < ReversePostOrderNodes.size(); ++i) {
    NodePositionMap[ReversePostOrderNodes.at(i)] = i;
  }
  return NodePositionMap;
}