#include "DeadCodeEliminationPass.h"
#include "DominatorTree.h"
#include "Module.h"
#include "NameGen.h"
#include <algorithm>
#include <array>
#include <iterator>
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
  static const array<InstructionType, 9> PreLive{
      InstructionType::Store, InstructionType::Move,  InstructionType::Param,
      InstructionType::Call,  InstructionType::Ret,   InstructionType::End,
      InstructionType::Read,  InstructionType::Write, InstructionType::WriteNL};

  DominatorTree ControlDependence(true);
  ControlDependence.buildDominatorTree(F);

  vector<Instruction *> WorkList;
  unordered_set<Value *> LiveSet;

  for (auto &BB : F.basicBlocks()) {
    for (auto &I : BB->instructions()) {
      // Mark all terminators live for now so we don't remove stuff needed for
      // Phi2Var.
      if (find(PreLive.begin(), PreLive.end(), I->InstrT) != PreLive.end() ||
          dynamic_cast<BasicBlockTerminator *>(I.get())) {
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
    auto &ControlDependencies = ControlDependence.DominanceFrontier[I->owner()];
    transform(ControlDependencies.begin(), ControlDependencies.end(),
              inserter(LiveSet, LiveSet.end()),
              [](BasicBlock *CD) { return CD->terminator(); });
    transform(ControlDependencies.begin(), ControlDependencies.end(),
              back_inserter(WorkList),
              [](BasicBlock *CD) { return CD->terminator(); });
  }

  return LiveSet;
}

void erase(Function &F, const unordered_set<Value *> &LiveValues) {
  for (auto &BB : F.basicBlocks()) {
    auto &Instructions = BB->instructions();
    Instructions.erase(
        remove_if(Instructions.begin(), Instructions.end(),
                  [&LiveValues](const auto &Instruction) {
                    bool IsDead =
                        LiveValues.find(Instruction.get()) == LiveValues.end();
                    bool IsNotTerminator = dynamic_cast<BasicBlockTerminator *>(
                                               Instruction.get()) == nullptr;
                    return IsDead && IsNotTerminator;
                  }),
        Instructions.end());
  }
}

BasicBlock *skipDeadBlocks(BasicBlock *BB,
                           const unordered_set<Value *> &LiveValues,
                           unordered_set<BasicBlock *> &VisitedBlocks) {
  auto Followers = BB->successors();

  while (!Followers.empty() && VisitedBlocks.find(BB) == VisitedBlocks.end()) {
    VisitedBlocks.insert(BB);

    if (auto ConditionalBranch =
            dynamic_cast<ConditionalBlockTerminator *>(BB->terminator())) {
      if (LiveValues.find(ConditionalBranch) == LiveValues.end()) {
        BB->fallthoughSuccessor() = ConditionalBranch->target();
        BB->releaseTerminator();
        Followers = BB->successors();
      } else {
        for (BasicBlock *Follower : Followers) {
          BB->updateSuccessor(
              Follower, skipDeadBlocks(Follower, LiveValues, VisitedBlocks));
        }
        break;
      }
    }

    if (auto Branch = dynamic_cast<BraInstruction *>(BB->terminator())) {
      if (BB->instructions().size() == 1 && BB->predecessors().size() <= 1) {
        // Disable removal of empty blocks for Phi2Var.
        // BB = Followers.front();
      } else {
        BB->updateSuccessor(
            Followers.front(),
            skipDeadBlocks(Followers.front(), LiveValues, VisitedBlocks));
        break;
      }
    } else if (BB->fallthoughSuccessor() != nullptr) {
      if (BB->instructions().size() == 0 && BB->predecessors().size() <= 1) {
        // Disable removal of empty blocks for Phi2Var.
        // BB = Followers.front();
      } else {
        BB->updateSuccessor(
            Followers.front(),
            skipDeadBlocks(Followers.front(), LiveValues, VisitedBlocks));
        break;
      }
    }

    Followers = BB->successors();
  }

  return BB;
} // namespace

void removeDeadBlocks(Function &F, BasicBlock *NewEntry) {
  stack<BasicBlock *> WorkingStack;
  unordered_set<BasicBlock *> MarkedBlocks;

  WorkingStack.push(NewEntry);

  while (!WorkingStack.empty()) {
    BasicBlock *Block = WorkingStack.top();
    WorkingStack.pop();
    MarkedBlocks.insert(Block);

    for (BasicBlock *Follower : Block->successors()) {
      if (MarkedBlocks.find(Follower) == MarkedBlocks.end()) {
        WorkingStack.push(Follower);
      }
    }
  }

  auto &BasicBlocks = F.basicBlocks();
  for_each(BasicBlocks.begin(), BasicBlocks.end(),
           [&MarkedBlocks](auto &Block) {
             if (MarkedBlocks.find(Block.get()) == MarkedBlocks.end()) {
               Block->releaseTerminator();
               Block->fallthoughSuccessor() = nullptr;
             }
           });
  BasicBlocks.erase(remove_if(BasicBlocks.begin(), BasicBlocks.end(),
                              [&MarkedBlocks](auto &Block) {
                                return MarkedBlocks.find(Block.get()) ==
                                       MarkedBlocks.end();
                              }),
                    BasicBlocks.end());
}

void trim(Function &F, const unordered_set<Value *> &LiveValues) {
  for (int I = 1; I <= 2; ++I) {
    unordered_set<BasicBlock *> VisitedBlocks;
    BasicBlock *NewEntry =
        skipDeadBlocks(F.entryBlock(), LiveValues, VisitedBlocks);
    removeDeadBlocks(F, NewEntry);
    auto NewEntryIt =
        find_if(F.basicBlocks().begin(), F.basicBlocks().end(),
                [NewEntry](auto &Block) { return Block.get() == NewEntry; });
    NewEntryIt->swap(F.basicBlocks().front());
  }
}
} // namespace

void DeadCodeEliminationPass::run(Function &F) {
  auto LiveValues = mark(F);
  erase(F, LiveValues);
  trim(F, LiveValues);
}
