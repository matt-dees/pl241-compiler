#include "DeadCodeEliminationPass.h"
#include "DominatorTree.h"
#include "Module.h"
#include "NameGen.h"
#include <algorithm>
#include <cassert>
#include <stack>
#include <unordered_set>
#include <vector>

using namespace cs241c;
using namespace std;

void DeadCodeEliminationPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(*F);
  }
}

namespace {
unordered_set<Value *> mark(Function &F) {
  DominatorTree ControlDependence(true);
  ControlDependence.buildDominatorTree(F);

  vector<Instruction *> WorkList;
  unordered_set<Value *> LiveSet;

  for (auto &BB : F.basicBlocks()) {
    for (auto &I : BB->instructions()) {
      if (I->isPreLive()) {
        LiveSet.insert(I.get());
        WorkList.push_back(I.get());
      }
    }
  }

  while (!WorkList.empty()) {
    Instruction *I = WorkList.back();
    WorkList.pop_back();

    for (Value *Arg : I->arguments()) {
      if (LiveSet.find(Arg) == LiveSet.end()) {
        LiveSet.insert(Arg);
        if (auto ArgI = dynamic_cast<Instruction *>(Arg)) {
          WorkList.push_back(ArgI);
        }
      }
    }

    auto &ControleDependencies = ControlDependence.DominanceFrontier[I->getOwner()];
    copy(ControleDependencies.begin(), ControleDependencies.end(), inserter(LiveSet, LiveSet.end()));
  }

  return LiveSet;
}
} // namespace

void DeadCodeEliminationPass::run(Function &F) { auto LiveValues = mark(F); }
