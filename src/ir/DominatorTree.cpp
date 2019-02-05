#include "DominatorTree.h"
#include "BasicBlock.h"
#include <algorithm>
#include <set>
#include <stack>
#include <unordered_set>

using namespace cs241c;
using namespace std;

void DominatorTree::buildDominatorTree(BasicBlock *Entry) {
  vector<BasicBlock *> ReversePostOrderCfg = reversePostOrder(Entry);

  IDomMap = createImmediateDomMap(ReversePostOrderCfg);
  DomTree = iDomMapToTree(IDomMap);
  DominanceFrontier = createDominanceFrontier(Entry, IDomMap);
}

unordered_multimap<BasicBlock *, BasicBlock *>
DominatorTree::iDomMapToTree(const unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
  unordered_multimap<BasicBlock *, BasicBlock *> LocalDomTree = {};
  for (auto IDomEntry : IDomMap) {
    if (IDomEntry.second != IDomEntry.first) {
      LocalDomTree.insert(make_pair(IDomEntry.second, IDomEntry.first));
    }
  }

  return LocalDomTree;
}

vector<BasicBlock *> DominatorTree::reversePostOrder(BasicBlock *Entry) {
  vector<BasicBlock *> Cfg = postOrder(Entry);
  reverse(Cfg.begin(), Cfg.end());
  return Cfg;
}

vector<BasicBlock *> DominatorTree::postOrder(BasicBlock *Entry) {
  vector<BasicBlock *> PostOrderNodes;
  unordered_set<BasicBlock *> Visited;
  return postOrder(Entry, PostOrderNodes, Visited);
}

std::vector<BasicBlock *> DominatorTree::postOrder(BasicBlock *Entry, vector<BasicBlock *> &PostOrderNodes,
                                                   unordered_set<BasicBlock *> &Visited) {
  if (Visited.find(Entry) != Visited.end()) {
    return PostOrderNodes;
  }
  Visited.insert(Entry);

  for (auto NextBlock : Entry->terminator()->followingBlocks()) {
    postOrder(NextBlock, PostOrderNodes, Visited);
  }
  PostOrderNodes.push_back(Entry);
  return PostOrderNodes;
}

unordered_map<BasicBlock *, BasicBlock *>
DominatorTree::createImmediateDomMap(const vector<BasicBlock *> &ReversePostOrderNodes) {
  unordered_map<BasicBlock *, BasicBlock *> IDoms;
  unordered_map<BasicBlock *, uint32_t> NodePositionMap = createNodePositionMap(ReversePostOrderNodes);

  bool Changed = true;
  BasicBlock *Entry = ReversePostOrderNodes.front();
  IDoms[Entry] = Entry;

  while (Changed) {
    Changed = false;
    for (auto Node : ReversePostOrderNodes) {
      if (Node->predecessors().empty()) {
        // Unreachable Node or start node. Will not include in dominator tree.
        continue;
      }
      BasicBlock *CandidateIDom = Node->predecessors().at(0);
      for (auto PredecessorIt = Node->predecessors().begin() + 1; PredecessorIt != Node->predecessors().end();
           PredecessorIt++) {
        if (IDoms.find(*PredecessorIt) != IDoms.end()) {
          CandidateIDom = intersect(*PredecessorIt, CandidateIDom, IDoms, NodePositionMap);
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

BasicBlock *DominatorTree::intersect(BasicBlock *Predecessor, BasicBlock *CandidateIDom,
                                     const unordered_map<BasicBlock *, BasicBlock *> &IDoms,
                                     const unordered_map<BasicBlock *, uint32_t> &NodePositionMap) {
  // Read page 7 of reference paper for description of intersect algorithm
  BasicBlock *Finger1 = Predecessor;
  BasicBlock *Finger2 = CandidateIDom;

  while (Finger1 != Finger2) {
    while (NodePositionMap.at(Finger1) > NodePositionMap.at(Finger2) && NodePositionMap.at(Finger1) != 0) {
      Finger1 = IDoms.at(Finger1);
    }
    while (NodePositionMap.at(Finger2) > NodePositionMap.at(Finger1) && NodePositionMap.at(Finger2) != 0) {
      Finger2 = IDoms.at(Finger2);
    }
  }
  return Finger1;
}

unordered_map<BasicBlock *, uint32_t>
DominatorTree::createNodePositionMap(const vector<BasicBlock *> &ReversePostOrderNodes) {
  unordered_map<BasicBlock *, uint32_t> NodePositionMap;
  for (uint32_t i = 0; i < ReversePostOrderNodes.size(); ++i) {
    NodePositionMap[ReversePostOrderNodes.at(i)] = i;
  }
  return NodePositionMap;
}

unordered_map<BasicBlock *, unordered_set<BasicBlock *>>
DominatorTree::createDominanceFrontier(BasicBlock *CurrentBlock,
                                       const unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
  static unordered_set<BasicBlock *> Visited;
  static unordered_map<BasicBlock *, unordered_set<BasicBlock *>> DF;
  Visited.insert(CurrentBlock);
  for (auto BB : CurrentBlock->predecessors()) {
    BasicBlock *Runner = BB;
    while (Runner != IDomMap.at(CurrentBlock) && Runner != CurrentBlock) {
      if (DF.find(Runner) == DF.end()) {
        DF[Runner] = {CurrentBlock};
      } else {
        DF.at(Runner).insert(CurrentBlock);
      }
      Runner = IDomMap.at(Runner);
    }
  }
  for (auto BB : CurrentBlock->terminator()->followingBlocks()) {
    if (Visited.find(BB) == Visited.end()) {
      createDominanceFrontier(BB, IDomMap);
    }
  }
  return DF;
}

unordered_set<BasicBlock *> DominatorTree::dominanceFrontier(cs241c::BasicBlock *BB) {
  if (DominanceFrontier.find(BB) == DominanceFrontier.end()) {
    return {};
  }
  return DominanceFrontier.at(BB);
}
