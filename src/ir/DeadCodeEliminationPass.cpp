#include "DeadCodeEliminationPass.h"
#include "DominatorTree.h"
#include "FunctionAnalyzer.h"
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
unordered_set<Value *> mark(Function &F, FunctionAnalyzer &FA) {
  static const array<InstructionType, 9> PreLive{
      InstructionType::Store, InstructionType::Move,  InstructionType::Param,
      InstructionType::Call,  InstructionType::Ret,   InstructionType::End,
      InstructionType::Read,  InstructionType::Write, InstructionType::WriteNL};

  DominatorTree ControlDependence(true);
  ControlDependence.buildDominatorTree(F);

  auto &DT = *FA.dominatorTree(&F);

  vector<Instruction *> WorkList;
  unordered_set<Value *> LiveSet;

  for (auto &BB : F.basicBlocks()) {
    for (auto &I : BB->instructions()) {
      if (find(PreLive.begin(), PreLive.end(), I->InstrT) != PreLive.end()) {
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
    transform(ControlDependencies.begin(), ControlDependencies.end(), inserter(LiveSet, LiveSet.end()),
              [](BasicBlock *CD) { return CD->terminator(); });
    transform(ControlDependencies.begin(), ControlDependencies.end(), back_inserter(WorkList),
              [](BasicBlock *CD) { return CD->terminator(); });

    if (I->InstrT == InstructionType::Phi) {
      auto CurrentBB = I->owner();
      Instruction *Terminator;

      if (CurrentBB->hasAttribute(BasicBlockAttr::While)) {
        Terminator = CurrentBB->terminator();
      } else {
        auto ImmediateDominator = DT.IDomMap[CurrentBB];
        Terminator = ImmediateDominator->terminator();
      }

      if (LiveSet.find(Terminator) == LiveSet.end()) {
        LiveSet.insert(Terminator);
        WorkList.push_back(Terminator);
      }
    }
  }

  return LiveSet;
}

void erase(Function &F, const unordered_set<Value *> &LiveValues) {
  for (auto &BB : F.basicBlocks()) {
    auto &Instructions = BB->instructions();
    Instructions.erase(remove_if(Instructions.begin(), Instructions.end(),
                                 [&LiveValues](const auto &Instruction) {
                                   bool IsDead = LiveValues.find(Instruction.get()) == LiveValues.end();
                                   bool IsNotTerminator = !isTerminator(Instruction->InstrT);
                                   return IsDead && IsNotTerminator;
                                 }),
                       Instructions.end());
  }
}

void removeDeadBlocks(Function &F) {
  stack<BasicBlock *> WorkingStack;
  unordered_set<BasicBlock *> MarkedBlocks;

  WorkingStack.push(F.entryBlock());

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
  for_each(BasicBlocks.begin(), BasicBlocks.end(), [&MarkedBlocks](auto &Block) {
    if (MarkedBlocks.find(Block.get()) == MarkedBlocks.end()) {
      Block->releaseTerminator();
      Block->fallthoughSuccessor() = nullptr;
    }
  });
  BasicBlocks.erase(
      remove_if(BasicBlocks.begin(), BasicBlocks.end(),
                [&MarkedBlocks](auto &Block) { return MarkedBlocks.find(Block.get()) == MarkedBlocks.end(); }),
      BasicBlocks.end());
}

void trim(Function &F, const unordered_set<Value *> &LiveValues) {
  for (auto &BB : F.basicBlocks()) {
    Instruction *Terminator = BB->terminator();
    if (Terminator != nullptr && isConditionalBranch(Terminator->InstrT)) {
      if (LiveValues.find(Terminator) == LiveValues.end()) {
        auto TerminatorPtr = BB->releaseTerminator();
        BB->fallthoughSuccessor() = TerminatorPtr->target();
      }
    }
  }
  removeDeadBlocks(F);
}

void cleanAttributes(Function &F) {
  for (auto &BB : F.basicBlocks()) {
    if (BB->predecessors().size() <= 1) {
      BB->removeAttribute(BasicBlockAttr::Join);
    }
    if (BB->successors().size() <= 1) {
      BB->removeAttribute(BasicBlockAttr::If);
      BB->removeAttribute(BasicBlockAttr::While);
    }
  }
}
} // namespace

void DeadCodeEliminationPass::run(Function &F) {
  auto LiveValues = mark(F, FA);
  erase(F, LiveValues);
  trim(F, LiveValues);
  cleanAttributes(F);
}
