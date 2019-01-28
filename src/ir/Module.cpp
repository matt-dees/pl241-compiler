#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;

Module::Module(std::string ModuleName,
               std::vector<std::unique_ptr<GlobalVariable>> &&Globals,
               std::vector<std::unique_ptr<Function>> &&Functions)
    : Name(move(ModuleName)), Globals(move(Globals)),
      Functions(std::move(Functions)),
      DT(Functions.at(0)->basicBlocks().at(0).get()) {}
std::string Module::getIdentifier() const { return Name; }
void Module::toSSA(IrGenContext &Ctx) {
  for (auto &F : Functions) {
    for (auto &BB : F->basicBlocks()) {
      insertPhiInstructions(BB.get(), Ctx);
    }
  }
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
