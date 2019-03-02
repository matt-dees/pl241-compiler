#include "KillPass.h"
#include "FunctionAnalyzer.h"
#include "Module.h"
#include "NameGen.h"

using namespace cs241c;

namespace {

void recurAddKillInDF(BasicBlock *BB, DominatorTree *DT) {
  for (auto DFEntry : DT->dominanceFrontier(BB)) {
    if (DFEntry->instructions().empty() || DFEntry->instructions().at(0)->InstrT == InstructionType::Kill) {
      continue;
    }
    auto Kill = std::make_unique<Instruction>(InstructionType::Kill, NameGen::genInstructionId());
    Kill->owner() = DFEntry;
    DFEntry->instructions().insert(DFEntry->instructions().begin(), std::move(Kill));
    recurAddKillInDF(DFEntry, DT);
  }
}

} // namespace

void KillPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(*F);
  }
}

void KillPass::run(Function &F) {
  for (auto &BB : F.basicBlocks()) {
    for (auto &Instr : BB->instructions()) {
      if (Instr->InstrT == InstructionType::Store) {
        recurAddKillInDF(BB.get(), FA.dominatorTree(&F));
      }
    }
  }
}
