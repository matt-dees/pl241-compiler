#include "DeadCodeEliminationPass.h"
#include "Module.h"
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace cs241c;
using namespace std;

void DeadCodeEliminationPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(*F);
  }
}

namespace {
void findLiveValues(unordered_set<Value *> &LiveValues, BasicBlock *B) {
  for (int I = static_cast<int>(B->instructions().size()) - 1; I >= 0; --I) {
    Instruction *Instr = B->instructions()[I].get();
    auto LiveValueIt = LiveValues.find(Instr);
    if (LiveValueIt != LiveValues.end()) {
      LiveValues.insert(Instr->arguments().begin(), Instr->arguments().end());
    } else if (Instr->isPreLive()) {
      LiveValues.insert(Instr->arguments().begin(), Instr->arguments().end());
    }
  }
}
} // namespace

void DeadCodeEliminationPass::run(Function &F) {
  stack<BasicBlock *> Blocks;
  unordered_set<BasicBlock *> VisitedBlocks;
  unordered_set<Value *> LiveValues;

  Blocks.push(F.entryBlock());

  while (!Blocks.empty()) {
    BasicBlock *B = Blocks.top();

    bool HasUnvisitedFollowers = false;
    for (auto Follower : B->terminator()->followingBlocks()) {
      if (VisitedBlocks.find(Follower) == VisitedBlocks.end()) {
        // If only some followers of my follower were already processed, the block was seen before, but not processed,
        // because it had children. This means that Follower is a loop header and only the follow was processed.
        auto FollowerFollowers = Follower->terminator()->followingBlocks();
        if (FollowerFollowers.size() == 2 &&
            FollowerFollowers.size() - count_if(FollowerFollowers.begin(), FollowerFollowers.end(),
                                                [&VisitedBlocks](BasicBlock *FollowerFollower) {
                                                  return VisitedBlocks.find(FollowerFollower) != VisitedBlocks.end();
                                                }) ==
                1) {
          findLiveValues(LiveValues, Follower);
        } else {
          HasUnvisitedFollowers = true;
          Blocks.push(Follower);
        }
      }
    }

    if (HasUnvisitedFollowers)
      continue;

    for (int I = static_cast<int>(B->instructions().size()) - 1; I >= 0; --I) {
      Instruction *Instr = B->instructions()[I].get();
      auto LiveValueIt = LiveValues.find(Instr);
      if (LiveValueIt != LiveValues.end()) {
        LiveValues.insert(Instr->arguments().begin(), Instr->arguments().end());
      } else if (Instr->isPreLive()) {
        LiveValues.insert(Instr->arguments().begin(), Instr->arguments().end());
      } else {
        B->instructions().erase(B->instructions().begin() + I);
      }
    }

    VisitedBlocks.insert(B);
    Blocks.pop();
  }
}
