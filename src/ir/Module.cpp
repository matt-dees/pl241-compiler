#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;

void Module::toSSA() {
  BasicBlock *Entry = Functions.at(0)->basicBlocks().at(0).get();
  SSAContext SSACtx;
  nodeToSSA(Entry, SSACtx);
}

SSAContext Module::nodeToSSA(BasicBlock *CurrentBB, SSAContext SSACtx) {
  static std::unordered_set<BasicBlock *> Visited = {};
  for (auto Pred : CurrentBB->Predecessors) {
    if (Visited.find(Pred) == Visited.end()) {
      // Not all predecessors have been explored.
      return SSACtx;
    }
  }
  if (Visited.find(CurrentBB) != Visited.end()) {
    // Already visited this node. Skip.
    return SSACtx;
  }
  CurrentBB->toSSA(SSACtx);
  Visited.insert(CurrentBB);
  for (auto BB : CurrentBB->Terminator->followingBlocks()) {
    SSAContext Ret = nodeToSSA(BB, SSACtx);
    SSACtx.merge(Ret);
  }
  return SSACtx;
}

Module::Module(std::string ModuleName) : Name(move(ModuleName)) {}

Value *Module::globalBase() { return &GlobalBase; }
std::string Module::getIdentifier() const { return Name; }

std::vector<std::unique_ptr<GlobalVariable>> &Module::globals() {
  return Globals;
}

const std::vector<std::unique_ptr<GlobalVariable>> &Module::globals() const {
  return Globals;
}

std::vector<std::unique_ptr<Function>> &Module::functions() {
  return Functions;
}

const std::vector<std::unique_ptr<Function>> &Module::functions() const {
  return Functions;
}

void Module::buildDominatorTree() {
  if (Functions.empty()) {
    return;
  }
  Function *FuncStart = Functions.at(0).get();
  if (FuncStart->basicBlocks().empty()) {
    return;
  }
  DT.buildDominatorTree(FuncStart->basicBlocks().at(0).get());
}
