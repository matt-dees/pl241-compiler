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
  std::unordered_set<BasicBlock *> Visited = {};
  recursiveGenAllPhis(F.entryBlock(), Visited);
  Visited.clear();
  recursiveNodeToSSA(F.entryBlock(), SSACtx, Visited);
}

SSAContext SSAPass::recursiveNodeToSSA(BasicBlock *CurrentBB, SSAContext SSACtx,
                                       std::unordered_set<BasicBlock *> &Visited) {
  if (Visited.find(CurrentBB) != Visited.end()) {
    // Already visited this node. Skip.
    return SSACtx;
  }

  basicBlockToSSA(*CurrentBB, SSACtx);
  Visited.insert(CurrentBB);

  for (auto VarChange : SSACtx.ssaVariableMap()) {
    propagateChangeToPhis(CurrentBB, VarChange.first, VarChange.second);
  }
  for (auto BB : CurrentBB->successors()) {
    SSAContext Ret = recursiveNodeToSSA(BB, SSACtx, Visited);
    SSACtx.merge(Ret);
  }
  return SSACtx;
}

void SSAPass::propagateChangeToPhis(cs241c::BasicBlock *SourceBB, cs241c::Variable *ChangedVar, cs241c::Value *NewVal) {
  for (auto BB : FA.dominatorTree(CurrentFunction)->dominanceFrontier(SourceBB)) {
    if (find(BB->predecessors().begin(), BB->predecessors().end(), SourceBB) != BB->predecessors().end()) {
      BB->updatePhiInst(SourceBB, ChangedVar, NewVal);
    }
  }
}

void SSAPass::recursiveGenAllPhis(BasicBlock *CurrentBB, std::unordered_set<BasicBlock *> &Visited) {
  if (Visited.find(CurrentBB) != Visited.end()) {
    return;
  }
  auto MoveTargets = CurrentBB->getMoveTargets();
  Visited.insert(CurrentBB);
  for (auto DFEntry : FA.dominatorTree(CurrentFunction)->dominanceFrontier(CurrentBB)) {
    for (auto &Target : MoveTargets) {
      auto Phi = std::make_unique<Instruction>(InstructionType::Phi, NameGen::genInstructionId(), Target, Target);
      Phi->storage() = Target;
      DFEntry->insertPhiInstruction(move(Phi));
      Visited.erase(DFEntry);
      recursiveGenAllPhis(DFEntry, Visited);
    }
  }
  for (auto BB : CurrentBB->successors()) {
    recursiveGenAllPhis(BB, Visited);
  }
}

void SSAPass::basicBlockToSSA(BasicBlock &BB, SSAContext &SSACtx) {
  // Remove MOVE instructions and update SSA context accordingly.
  for (auto InstIter = BB.instructions().begin(); InstIter != BB.instructions().end();) {
    if (InstIter->get()->InstrT == InstructionType::Move) {
      if (auto Target = (dynamic_cast<Variable *>(InstIter->get()->arguments()[1].R.Ptr))) {
        SSACtx.updateVariable(Target, InstIter->get()->arguments()[0]);
      }
      InstIter = BB.instructions().erase(InstIter);
      continue;
    }
    (*InstIter)->updateArgs(SSACtx);
    if (InstIter->get()->InstrT == InstructionType::Phi) {
      SSACtx.updateVariable((*InstIter)->storage(), InstIter->get());
    }
    InstIter++;
  }
}