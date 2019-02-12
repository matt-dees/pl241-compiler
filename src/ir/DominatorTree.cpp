#include "DominatorTree.h"
#include "BasicBlock.h"
#include "Function.h"
#include <algorithm>
#include <iterator>
#include <stack>
#include <vector>

using namespace cs241c;
using namespace std;

namespace {
class DominatorTreeBuilder {
  bool Reversed;

  vector<BasicBlock *> predecessors(BasicBlock &Block) {
    return !Reversed ? Block.predecessors() : Block.terminator()->followingBlocks();
  }

  vector<BasicBlock *> followers(BasicBlock &Block) {
    return !Reversed ? Block.terminator()->followingBlocks() : Block.predecessors();
  }

public:
  DominatorTreeBuilder(bool Reversed) : Reversed(Reversed) {}

  vector<BasicBlock *> postOrder(const vector<BasicBlock *> &EntryBlocks) {
    stack<BasicBlock *> WorkStack;
    unordered_set<BasicBlock *> Visited;
    vector<BasicBlock *> Result;

    for (BasicBlock *EntryBlock : EntryBlocks) {
      WorkStack.push(EntryBlock);
    }

    while (!WorkStack.empty()) {
      BasicBlock *B = WorkStack.top();
      Visited.insert(B);

      bool HasUnvisitedFollower = false;
      for (BasicBlock *Follower : followers(*B)) {
        if (Visited.find(Follower) == Visited.end()) {
          HasUnvisitedFollower = true;
          WorkStack.push(Follower);
        }
      }

      if (HasUnvisitedFollower)
        continue;

      WorkStack.pop();
      Result.push_back(B);
    }

    return Result;
  }

  vector<BasicBlock *> reversePostOrder(const std::vector<BasicBlock *> &EntryBlocks) {
    vector<BasicBlock *> Cfg = postOrder(EntryBlocks);
    reverse(Cfg.begin(), Cfg.end());
    return Cfg;
  }

  unordered_multimap<BasicBlock *, BasicBlock *>
  iDomMapToTree(const unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
    unordered_multimap<BasicBlock *, BasicBlock *> LocalDomTree = {};
    for (auto IDomEntry : IDomMap) {
      if (IDomEntry.second != IDomEntry.first) {
        LocalDomTree.insert(make_pair(IDomEntry.second, IDomEntry.first));
      }
    }

    return LocalDomTree;
  }

  unordered_map<BasicBlock *, uint32_t> createNodePositionMap(const vector<BasicBlock *> &ReversePostOrderNodes) {
    unordered_map<BasicBlock *, uint32_t> NodePositionMap;
    for (uint32_t i = 0; i < ReversePostOrderNodes.size(); ++i) {
      NodePositionMap[ReversePostOrderNodes.at(i)] = i;
    }
    return NodePositionMap;
  }

  BasicBlock *intersect(BasicBlock *Predecessor, BasicBlock *CandidateIDom,
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

  unordered_map<BasicBlock *, BasicBlock *> createImmediateDomMap(const vector<BasicBlock *> &ReversePostOrderNodes) {
    unordered_map<BasicBlock *, BasicBlock *> IDoms;
    unordered_map<BasicBlock *, uint32_t> NodePositionMap = createNodePositionMap(ReversePostOrderNodes);

    bool Changed = true;
    BasicBlock *Entry = ReversePostOrderNodes.front();
    IDoms[Entry] = Entry;

    while (Changed) {
      Changed = false;
      for (auto Node : ReversePostOrderNodes) {
        vector<BasicBlock *> Predecessors = predecessors(*Node);
        if (Predecessors.empty()) {
          // Unreachable Node or start node. Will not include in dominator tree.
          continue;
        }
        BasicBlock *CandidateIDom = Predecessors.at(0);
        auto PredecessorsEnd = Predecessors.end();
        for (auto PredecessorIt = Predecessors.begin() + 1; PredecessorIt != PredecessorsEnd; PredecessorIt++) {
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

  void createDominanceFrontierRec(BasicBlock *CurrentBlock, const unordered_map<BasicBlock *, BasicBlock *> &IDomMap,
                                  unordered_set<BasicBlock *> &Visited,
                                  unordered_map<BasicBlock *, unordered_set<BasicBlock *>> &DF) {
    Visited.insert(CurrentBlock);
    for (auto BB : predecessors(*CurrentBlock)) {
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
    for (auto BB : followers(*CurrentBlock)) {
      if (Visited.find(BB) == Visited.end()) {
        createDominanceFrontierRec(BB, IDomMap, Visited, DF);
      }
    }
  }

  unordered_map<BasicBlock *, unordered_set<BasicBlock *>>
  createDominanceFrontier(std::vector<BasicBlock *> EntryBlocks,
                          const unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
    unordered_set<BasicBlock *> Visited;
    unordered_map<BasicBlock *, unordered_set<BasicBlock *>> DF;
    for (auto BB : EntryBlocks) {
      createDominanceFrontierRec(BB, IDomMap, Visited, DF);
    }
    return DF;
  }

  vector<BasicBlock *> findEntryBlocks(Function &F) {
    vector<BasicBlock *> EntryBlocks;

    for (auto &Block : F.basicBlocks()) {
      if (predecessors(*Block).empty()) {
        EntryBlocks.push_back(Block.get());
      }
    }

    return EntryBlocks;
  }
};

} // namespace

DominatorTree::DominatorTree(bool Reverse) : Reverse(Reverse) {}

void DominatorTree::buildDominatorTree(Function &F) {
  DominatorTreeBuilder DTB(Reverse);
  vector<BasicBlock *> EntryBlocks = DTB.findEntryBlocks(F);
  vector<BasicBlock *> ReversePostOrderCfg = DTB.reversePostOrder(EntryBlocks);

  IDomMap = DTB.createImmediateDomMap(ReversePostOrderCfg);
  DomTree = DTB.iDomMapToTree(IDomMap);
  DominanceFrontier = DTB.createDominanceFrontier(EntryBlocks, IDomMap);
}

unordered_set<BasicBlock *> DominatorTree::dominanceFrontier(BasicBlock *BB) {
  if (DominanceFrontier.find(BB) == DominanceFrontier.end()) {
    return {};
  }
  return DominanceFrontier.at(BB);
}

bool DominatorTree::doesBlockDominate(BasicBlock *Dominator, BasicBlock *Candidate) const {
  if (Dominator == Candidate) {
    return true;
  }
  BasicBlock *Runner = Candidate;
  while (IDomMap.find(Runner) != IDomMap.end() && IDomMap.at(Runner) != Runner) {
    if (IDomMap.at(Runner) == Dominator) {
      return true;
    }
    Runner = IDomMap.at(Runner);
  }
  return false;
}
