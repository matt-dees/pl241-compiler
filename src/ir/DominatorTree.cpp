#include "DominatorTree.h"
#include <algorithm>
#include <set>
#include <stack>
#include <unordered_set>

using namespace cs241c;

DominatorTree::DominatorTree(cs241c::BasicBlock *CfgEntry) {
  buildDominatorTree(CfgEntry);
}

void DominatorTree::buildDominatorTree(BasicBlock *Entry) {
  std::vector<BasicBlock *> ReversePostOrderCfg = reversePostOrder(Entry);

  IDomMap = createImmediateDomMap(ReversePostOrderCfg);
  DomTree = iDomMapToTree(IDomMap);
  DominanceFrontier = createDominanceFrontier(Entry, IDomMap);
}

std::unordered_multimap<BasicBlock *, BasicBlock *>
DominatorTree::iDomMapToTree(
    const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
  std::unordered_multimap<BasicBlock *, BasicBlock *> LocalDomTree = {};
  for (auto IDomEntry : IDomMap) {
    if (IDomEntry.second != IDomEntry.first) {
      LocalDomTree.insert(std::make_pair(IDomEntry.second, IDomEntry.first));
    }
  }

  return LocalDomTree;
}

std::vector<BasicBlock *> DominatorTree::reversePostOrder(BasicBlock *Entry) {
  std::vector<BasicBlock *> Cfg = postOrder(Entry);
  std::reverse(Cfg.begin(), Cfg.end());
  return Cfg;
}

std::vector<BasicBlock *> DominatorTree::postOrder(BasicBlock *Entry) {
  static std::vector<BasicBlock *> PostOrderNodes;
  static std::unordered_set<BasicBlock *> Visited;

  for (auto NextBlock : Entry->Terminator->followingBlocks()) {
    postOrder(NextBlock);
  }
  if (Visited.find(Entry) == Visited.end()) {
    PostOrderNodes.push_back(Entry);
    Visited.insert(Entry);
  }
  return PostOrderNodes;
}

std::unordered_map<BasicBlock *, BasicBlock *>
DominatorTree::createImmediateDomMap(
    const std::vector<BasicBlock *> &ReversePostOrderNodes) {
  std::unordered_map<BasicBlock *, BasicBlock *> IDoms;
  std::unordered_map<BasicBlock *, uint32_t> NodePositionMap =
      createNodePositionMap(ReversePostOrderNodes);

  bool Changed = true;
  BasicBlock *Entry = ReversePostOrderNodes.front();
  IDoms[Entry] = Entry;

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
              intersect(*PredecessorIt, CandidateIDom, IDoms, NodePositionMap);
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
    while (NodePositionMap.at(Finger1) > NodePositionMap.at(Finger2) &&
           NodePositionMap.at(Finger1) != 0) {
      Finger1 = IDoms.at(Finger1);
    }
    while (NodePositionMap.at(Finger2) > NodePositionMap.at(Finger1) &&
           NodePositionMap.at(Finger2) != 0) {
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

std::unordered_multimap<BasicBlock *, BasicBlock *>
DominatorTree::createDominanceFrontier(
    BasicBlock *CurrentBlock,
    const std::unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
  static std::unordered_multimap<BasicBlock *, BasicBlock *> DF;
  if (CurrentBlock->Predecessors.size() > 1) {
    return DF;
  }
  for (auto BB : CurrentBlock->Predecessors) {
    BasicBlock *Runner = BB;
    while (Runner != IDomMap.at(CurrentBlock)) {
      DF.insert(std::make_pair(Runner, CurrentBlock));
      Runner = IDomMap.at(Runner);
    }
  }
  for (auto BB : CurrentBlock->Terminator->followingBlocks()) {
    createDominanceFrontier(BB, IDomMap);
  }
  return DF;
}