#include "Phi2VarPass.h"
#include "Module.h"

using namespace cs241c;

namespace {
void process(Function &F) {
  auto &Registers = F.registerColoring();

  for (auto &BB : F.basicBlocks()) {
    auto &Predecessors = BB->predecessors();

    for (auto &Instr : BB->instructions()) {
      if (Instr->InstrT != InstructionType::Phi)
        break;

      auto Phi = dynamic_cast<PhiInstruction *>(Instr.get());
      auto ResultReg = Registers[Phi];
    }
  }
}
} // namespace

void Phi2VarPass::run(Module &M) {
  for (auto &F : M.functions()) {
    process(*F);
  }
}