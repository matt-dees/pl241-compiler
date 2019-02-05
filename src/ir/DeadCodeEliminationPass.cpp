#include "DeadCodeEliminationPass.h"
#include "Module.h"
#include <stack>
#include <unordered_set>

using namespace cs241c;
using namespace std;

void DeadCodeEliminationPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(*F);
  }
}

void DeadCodeEliminationPass::run(Function &F) {
  stack<BasicBlock *> Blocks;
  unordered_set<BasicBlock *> VisitedBlocks;
  unordered_set<Value *> LiveValues;

  Blocks.push(F.entryBlock());

  while (!Blocks.empty()) {
    BasicBlock *B = Blocks.top();
    VisitedBlocks.insert(B);

    bool HasUnvisitedFollowers = false;
    for (auto Follower : B->terminator()->followingBlocks()) {
      if (VisitedBlocks.find(Follower) == VisitedBlocks.end()) {
        HasUnvisitedFollowers = true;
        Blocks.push(Follower);
      }
    }

    if (HasUnvisitedFollowers)
      continue;

    auto BlockREnd = B->instructions().rend();
    for (auto It = B->instructions().rbegin(); It != BlockREnd; ++It) {
      Instruction *I = It->get();
      auto LiveValueIt = LiveValues.find(I);
      if (LiveValueIt != LiveValues.end()) {
        LiveValues.insert(I->arguments().begin(), I->arguments().end());
        LiveValues.erase(LiveValueIt);
      } else if (I->isPreLive()) {
        LiveValues.insert(I->arguments().begin(), I->arguments().end());
      } else {
        B->instructions().erase(It.base());
      }
    }

    Blocks.pop();
  }
}
