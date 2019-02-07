#include "DeadCodeEliminationPass.h"
#include "Module.h"
#include "NameGen.h"
#include <algorithm>
#include <cassert>
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
void findLiveValues(unordered_set<Value *> &LiveValues, BasicBlock *B, bool RemoveDead) {
  for (int I = static_cast<int>(B->instructions().size()) - 1; I >= 0; --I) {
    Instruction *Instr = B->instructions()[I].get();
    auto LiveValueIt = LiveValues.find(Instr);
    if (LiveValueIt != LiveValues.end()) {
      LiveValues.insert(Instr->arguments().begin(), Instr->arguments().end());
    } else if (Instr->isPreLive()) {
      LiveValues.insert(Instr->arguments().begin(), Instr->arguments().end());
    } else if (RemoveDead) {
      B->instructions().erase(B->instructions().begin() + I);
    }
  }
}

bool pushFollowers(stack<BasicBlock *> &Blocks, unordered_set<BasicBlock *> &VisitedBlocks,
                   unordered_set<Value *> &LiveValues, BasicBlock *B) {
  bool HasUnvisitedFollowers = false;
  for (auto Follower : B->terminator()->followingBlocks()) {
    if (VisitedBlocks.find(Follower) == VisitedBlocks.end()) {
      // If only some followers of my follower were already processed, the block was seen before, but not processed,
      // because it had children. This means that Follower is a loop header and only the follow was processed.
      auto FollowerFollowers = Follower->terminator()->followingBlocks();
      if (FollowerFollowers.size() == 2 &&
          count_if(FollowerFollowers.begin(), FollowerFollowers.end(), [&VisitedBlocks](BasicBlock *FollowerFollower) {
            return VisitedBlocks.find(FollowerFollower) != VisitedBlocks.end();
          }) == 1) {
        findLiveValues(LiveValues, Follower, false);
      } else {
        HasUnvisitedFollowers = true;
        Blocks.push(Follower);
      }
    }
  }
  return HasUnvisitedFollowers;
}

bool blockIsEmpty(BasicBlock *B) {
  assert(!B->instructions().empty() && "BasicBlock must have at least a terminator.");
  return B->instructions().size() == 1;
}

void removeEmptyBlocks(unordered_set<BasicBlock *> &VisitedBlocks, unordered_set<BasicBlock *> &BlocksToRemove,
                       BasicBlock *B) {
  if (auto ConditionalTermiantor = dynamic_cast<ConditionalBlockTerminator *>(B->terminator())) {
    auto FollowingBlocks = ConditionalTermiantor->followingBlocks();

    for (auto Follower : FollowingBlocks) {
      if (VisitedBlocks.find(Follower) == VisitedBlocks.end()) {
        continue;
      }

      auto FollowerTerminator = Follower->terminator();
      auto FollowerFollowers = FollowerTerminator->followingBlocks();
      if (blockIsEmpty(Follower) && Follower->predecessors().size() == 1 && FollowerFollowers.size() == 1) {
        ConditionalTermiantor->updateTarget(Follower, FollowerFollowers[0]);
        BlocksToRemove.insert(Follower);
      }
    }

    FollowingBlocks = ConditionalTermiantor->followingBlocks();
    if (FollowingBlocks[0] == FollowingBlocks[1]) {
      auto &FollowPred = FollowingBlocks[0]->predecessors();
      FollowPred.erase(remove(FollowPred.begin(), FollowPred.end(), B), FollowPred.end());
      B->terminate(make_unique<BraInstruction>(NameGen::genInstructionId(), FollowingBlocks[0]));
    }
  } else if (auto BranchTerminator = dynamic_cast<BraInstruction *>(B->terminator())) {
    auto Follower = BranchTerminator->followingBlocks()[0];

    if (VisitedBlocks.find(Follower) != VisitedBlocks.end() && blockIsEmpty(Follower)) {
      // Currently we only merge if the follower has no additional predecessors.
      if (Follower->predecessors().size() == 1) {
        unique_ptr<BasicBlockTerminator> Terminator = Follower->releaseTerminator();
        B->terminate(move(Terminator));
        BlocksToRemove.insert(Follower);
      }
    }
  }
}
} // namespace

void DeadCodeEliminationPass::run(Function &F) {
  stack<BasicBlock *> Blocks;
  unordered_set<BasicBlock *> VisitedBlocks;
  unordered_set<Value *> LiveValues;
  unordered_set<BasicBlock *> BlocksToRemove;

  Blocks.push(F.entryBlock());

  while (!Blocks.empty()) {
    BasicBlock *B = Blocks.top();

    bool HasUnvisitedFollowers = pushFollowers(Blocks, VisitedBlocks, LiveValues, B);

    if (HasUnvisitedFollowers)
      continue;

    removeEmptyBlocks(VisitedBlocks, BlocksToRemove, B);

    findLiveValues(LiveValues, B, true);

    VisitedBlocks.insert(B);
    Blocks.pop();
  }

  F.basicBlocks().erase(
      remove_if(F.basicBlocks().begin(), F.basicBlocks().end(),
                [&BlocksToRemove](auto &B) { return BlocksToRemove.find(B.get()) != BlocksToRemove.end(); }),
      F.basicBlocks().end());
}
