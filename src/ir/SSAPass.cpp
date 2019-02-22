#include "SSAPass.h"
#include "Instruction.h"
#include "NameGen.h"
#include <algorithm>
#include <unordered_set>

using namespace cs241c;

void SSAPass::run(Module &M) {
  for (auto &F : M.functions()) {
    this->CurrentFunction = F.get();
    run(*F);
  }
}

void SSAPass::run(Function &F) {
  SSAContext SSACtx;
  recursiveGenAllPhis(F.entryBlock());
  recursiveNodeToSSA(F.entryBlock(), SSACtx);
}

SSAContext SSAPass::recursiveNodeToSSA(BasicBlock *CurrentBB,
                                       SSAContext SSACtx) {
  static std::unordered_set<BasicBlock *> Visited = {};
  if (Visited.find(CurrentBB) != Visited.end()) {
    // Already visited this node. Skip.
    return SSACtx;
  }

  CurrentBB->toSSA(SSACtx);
  Visited.insert(CurrentBB);

  for (auto VarChange : SSACtx.ssaVariableMap()) {
    propagateChangeToPhis(CurrentBB, VarChange.first, VarChange.second);
  }
  for (auto BB : CurrentBB->successors()) {
    SSAContext Ret = recursiveNodeToSSA(BB, SSACtx);
    SSACtx.merge(Ret);
  }
  return SSACtx;
}

void SSAPass::propagateChangeToPhis(cs241c::BasicBlock *SourceBB,
                                    cs241c::Variable *ChangedVar,
                                    cs241c::Value *NewVal) {
  for (auto BB :
       FA.dominatorTree(CurrentFunction)->dominanceFrontier(SourceBB)) {
    if (find(BB->predecessors().begin(), BB->predecessors().end(), SourceBB) !=
        BB->predecessors().end()) {
      BB->updatePhiInst(SourceBB, ChangedVar, NewVal);
    }
  }
}

void SSAPass::recursiveGenAllPhis(BasicBlock *CurrentBB) {
  static std::unordered_set<BasicBlock *> Visited;
  if (Visited.find(CurrentBB) != Visited.end()) {
    return;
  }
  auto MoveTargets = CurrentBB->getMoveTargets();
  Visited.insert(CurrentBB);
  for (auto DFEntry :
       FA.dominatorTree(CurrentFunction)->dominanceFrontier(CurrentBB)) {
    for (auto &Target : MoveTargets) {
      auto Phi = std::make_unique<Instruction>(
          InstructionType::Phi, NameGen::genInstructionId(), Target, Target);
      Phi->storage() = Target;
      DFEntry->insertPhiInstruction(move(Phi));
      Visited.erase(DFEntry);
      recursiveGenAllPhis(DFEntry);
    }
  }
  for (auto BB : CurrentBB->successors()) {
    recursiveGenAllPhis(BB);
  }
}