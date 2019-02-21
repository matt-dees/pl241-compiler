#include "Function.h"
#include "RegisterAllocator.h"
#include <algorithm>
#include <iterator>
#include <stack>
#include <unordered_set>

using namespace cs241c;
using namespace std;

Function::Function(string Name, vector<unique_ptr<LocalVariable>> &&Locals)
    : Value(ValueType::Function), Name(move(Name)), Locals(move(Locals)) {}

vector<unique_ptr<ConstantValue>> &Function::constants() { return Constants; }

vector<unique_ptr<LocalVariable>> &Function::locals() { return Locals; }

BasicBlock *Function::entryBlock() const { return BasicBlocks.front().get(); }

vector<unique_ptr<BasicBlock>> &Function::basicBlocks() { return BasicBlocks; }
const vector<unique_ptr<BasicBlock>> &Function::basicBlocks() const {
  return BasicBlocks;
}

string Function::toString() const { return Name; }

void Function::buildDominatorTree() {
  if (basicBlocks().empty()) {
    return;
  }
  DT.buildDominatorTree(*this);
}

DominatorTree &Function::dominatorTree() { return DT; }

void Function::toSSA(IrGenContext &GenCtx) {
  SSAContext SSACtx;
  recursiveGenAllPhis(entryBlock(), GenCtx);
  recursiveNodeToSSA(entryBlock(), SSACtx);
}

SSAContext Function::recursiveNodeToSSA(BasicBlock *CurrentBB,
                                        SSAContext SSACtx) {
  static unordered_set<BasicBlock *> Visited = {};
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

void Function::propagateChangeToPhis(cs241c::BasicBlock *SourceBB,
                                     cs241c::Variable *ChangedVar,
                                     cs241c::Value *NewVal) {
  for (auto BB : DT.dominanceFrontier(SourceBB)) {
    if (find(BB->predecessors().begin(), BB->predecessors().end(), SourceBB) !=
        BB->predecessors().end()) {
      BB->updatePhiInst(SourceBB, ChangedVar, NewVal);
    }
  }
}

void Function::recursiveGenAllPhis(BasicBlock *CurrentBB,
                                   IrGenContext &PhiGenCtx) {
  static unordered_set<BasicBlock *> Visited;
  if (Visited.find(CurrentBB) != Visited.end()) {
    return;
  }
  auto MoveTargets = CurrentBB->getMoveTargets();
  Visited.insert(CurrentBB);
  for (auto DFEntry : dominatorTree().dominanceFrontier(CurrentBB)) {
    for (auto &Target : MoveTargets) {
      PhiGenCtx.currentBlock() = DFEntry;
      PhiGenCtx.makePhiInstruction(Target, Target, Target);
      Visited.erase(DFEntry);
      recursiveGenAllPhis(DFEntry, PhiGenCtx);
    }
  }
  for (auto BB : CurrentBB->successors()) {
    recursiveGenAllPhis(BB, PhiGenCtx);
  }
}

std::vector<BasicBlock *> Function::postOrderCfg() {
  stack<BasicBlock *> WorkStack;
  unordered_set<BasicBlock *> Visited;
  std::vector<BasicBlock *> PostOrdering;

  WorkStack.push(entryBlock());

  while (!WorkStack.empty()) {
    BasicBlock *B = WorkStack.top();
    Visited.insert(B);

    bool HasUnvisitedFollower = false;
    for (BasicBlock *Follower : B->successors()) {
      if (Visited.find(Follower) == Visited.end()) {
        HasUnvisitedFollower = true;
        WorkStack.push(Follower);
      }
    }

    if (HasUnvisitedFollower)
      continue;

    WorkStack.pop();
    PostOrdering.push_back(B);
  }
  return PostOrdering;
}

bool Function::isLoopHdrBlock(BasicBlock *BB) {
  for (auto Pred : BB->predecessors()) {
    // Loop header blocks dominate one of their predecessors.
    if (DT.doesBlockDominate(BB, Pred)) {
      return true;
    }
  }
}

bool Function::isJoinBlock(BasicBlock *BB) {
  if (BB->predecessors().size() != 2) {
    return false;
  }
  for (auto Pred : BB->predecessors()) {
    // Join blocks do not dominate their predecessors.
    if (DT.doesBlockDominate(BB, Pred)) {
      return false;
    }
  }
  return true;
}

void Function::assignRegisters() {
  RegisterAllocator RA;
  RAColoring = RA.color(IG);
}

void Function::buildInterferenceGraph() {
  IGBuilder IGB(this);
  IGB.buildInterferenceGraph();
  IG = IGB.interferenceGraph();
}