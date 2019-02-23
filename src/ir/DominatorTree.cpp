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

  vector<BasicBlock *> predecessors(BasicBlock &Block) { return !Reversed ? Block.predecessors() : Block.successors(); }

  vector<BasicBlock *> followers(BasicBlock &Block) { return !Reversed ? Block.successors() : Block.predecessors(); }

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

  vector<BasicBlock *> reversePostOrder(const vector<BasicBlock *> &EntryBlocks) {
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

    bool Changed;
    while (Finger1 != Finger2) {
      Changed = false;
      while (NodePositionMap.at(Finger1) > NodePositionMap.at(Finger2) && NodePositionMap.at(Finger1) != 0) {
        BasicBlock *Finger1Dom = IDoms.at(Finger1);
        if (Finger1Dom != Finger1) {
          Finger1 = IDoms.at(Finger1);
          Changed = true;
        } else {
          break;
        }
      }

      while (NodePositionMap.at(Finger2) > NodePositionMap.at(Finger1) && NodePositionMap.at(Finger2) != 0) {
        BasicBlock *Finger2Dom = IDoms.at(Finger2);
        if (Finger2Dom != Finger2) {
          Finger2 = IDoms.at(Finger2);
          Changed = true;
        } else {
          break;
        }
      }

      if (!Changed) {
        return nullptr;
      }
    }
    return Finger1;
  }

  unordered_map<BasicBlock *, BasicBlock *> createImmediateDomMap(const vector<BasicBlock *> &ReversePostOrderNodes,
                                                                  const vector<BasicBlock *> &EntryBlocks) {
    unordered_map<BasicBlock *, BasicBlock *> IDoms;
    unordered_map<BasicBlock *, uint32_t> NodePositionMap = createNodePositionMap(ReversePostOrderNodes);

    bool Changed = true;
    for (auto Entry : EntryBlocks) {
      IDoms[Entry] = Entry;
    }

    while (Changed) {
      Changed = false;
      for (auto Node : ReversePostOrderNodes) {
        vector<BasicBlock *> Predecessors = predecessors(*Node);
        if (Predecessors.empty()) {
          // Unreachable Node or start node. Will not include in dominator tree.
          continue;
        }
        BasicBlock *CandidateIDom = *find_if(Predecessors.begin(), Predecessors.end(),
                                             [&IDoms](BasicBlock *Block) { return IDoms.find(Block) != IDoms.end(); });
        auto PredecessorsEnd = Predecessors.end();
        for (auto PredecessorIt = Predecessors.begin(); PredecessorIt != PredecessorsEnd; ++PredecessorIt) {
          if (*PredecessorIt == CandidateIDom)
            continue;
          if (IDoms.find(*PredecessorIt) != IDoms.end()) {
            CandidateIDom = intersect(*PredecessorIt, CandidateIDom, IDoms, NodePositionMap);
            if (CandidateIDom == nullptr) {
              CandidateIDom = Node;
              break;
            }
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

  unordered_map<BasicBlock *, unordered_set<BasicBlock *>>
  createDominanceFrontier(Function &F, const unordered_map<BasicBlock *, BasicBlock *> &IDomMap) {
    unordered_map<BasicBlock *, unordered_set<BasicBlock *>> DF;

    for (auto &Block : F.basicBlocks()) {
      auto Preds = predecessors(*Block);

      if (Preds.size() <= 1)
        continue;

      for (auto Pred : Preds) {
        BasicBlock *Runner = Pred;
        while (Runner != IDomMap.at(Block.get()) && Runner != Block.get()) {
          if (DF.find(Runner) == DF.end()) {
            DF[Runner] = {Block.get()};
          } else {
            DF[Runner].insert(Block.get());
          }
          BasicBlock *RunnerDom = IDomMap.at(Runner);
          if (RunnerDom != Runner) {
            Runner = RunnerDom;
          } else {
            break;
          }
        }
      }
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

  IDomMap = DTB.createImmediateDomMap(ReversePostOrderCfg, EntryBlocks);
  DomTree = DTB.iDomMapToTree(IDomMap);
  DominanceFrontier = DTB.createDominanceFrontier(F, IDomMap);
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
