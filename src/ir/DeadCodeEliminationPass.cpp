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
  stack<Instruction *> WorkStack;
  unordered_set<Instruction *> LiveSet;

  for (auto &BB : F.basicBlocks()) {
    for (auto &I : BB->instructions()) {
      if (I->isPreLive()) {
        LiveSet.emplace(I.get());
        WorkStack.push(I.get());
      }
    }
  }

  while (!WorkStack.empty()) {
    auto I = WorkStack.top();
    WorkStack.pop();
    for (auto Value : I->arguments()) {
      if (auto Definer = dynamic_cast<Instruction *>(Value)) {
        if (LiveSet.find(Definer) == LiveSet.end()) {
          LiveSet.insert(Definer);
          WorkStack.push(Definer);
		}
	  }
	}
  }
}
