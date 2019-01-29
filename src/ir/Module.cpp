#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;

Module::Module(std::string ModuleName,
               std::vector<std::unique_ptr<GlobalVariable>> &&Globals,
               std::vector<std::unique_ptr<Function>> &&Functions)
    : Name(move(ModuleName)), Globals(move(Globals)),
      Functions(std::move(Functions)),
      DT(this->Functions.at(0)->basicBlocks().at(0).get()) {}
std::string Module::getIdentifier() const { return Name; }
void Module::toSSA(IrGenContext &Ctx) {

  // Conversion to SSA requires two passes:
  // (1) Insert PHI nodes in correct locations according to dominance frontier.
  // (2) Convert basic blocks to SSA form (remove MOVE instructions). This must
  // happen after the PHI nodes have been added because some arguments will need
  // to be updated to the value of the newly added PHI instructions.
  for (auto &F : Functions) {
    for (auto &BB : F->basicBlocks()) {
      insertPhiInstructions(BB.get(), Ctx);
    }
  }

  // 2nd Pass. Use recursive function to traverse CFG.
  BasicBlock *Entry = Functions.at(0)->basicBlocks().at(0).get();
  SSAContext SSACtx;
  cfgToSSA(Entry, SSACtx);
}

SSAContext Module::cfgToSSA(BasicBlock *CurrentBB, SSAContext SSACtx) {
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
    SSAContext Ret = cfgToSSA(BB, SSACtx);
    SSACtx.merge(Ret);
  }
  return SSACtx;
}

void Module::insertPhiInstructions(cs241c::BasicBlock *BB, IrGenContext &Ctx) {
  for (auto I : *BB) {
    if (auto MI = dynamic_cast<MoveInstruction *>(I)) {
      if (auto Var = dynamic_cast<Variable *>(MI->Target)) {
        for (auto DFEntry : DT.dominanceFrontier(BB)) {
          DFEntry->insertPhiInstruction(Var, MI->Source, Ctx.genInstructionId(),
                                        BB);
        }
      }
    }
  }
}
