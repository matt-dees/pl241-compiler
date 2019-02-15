#include "RegisterAllocation.h"
#include <iostream>

using namespace cs241c;
using namespace std;

InterferenceGraph RegisterAllocation::buildInterferenceGraph(Function &F) {
  InterferenceGraph IG;
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>>
      PredecessorLiveSetMap;
  for (auto BBIter : F.postOrderCfg()) {
    for (auto ReverseInstructionIt = BBIter->instructions().rbegin();
         ReverseInstructionIt != BBIter->instructions().rend();
         ReverseInstructionIt++) {
      std::cout << ReverseInstructionIt->get()->toString() << "\n";
    }
  }
  return IG;
}

void InterferenceGraph::writeGraph(ofstream &) { return; }
