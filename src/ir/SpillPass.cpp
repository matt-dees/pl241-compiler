#include "SpillPass.h"
#include "FunctionAnalyzer.h"
#include "Module.h"
#include "NameGen.h"

using namespace cs241c;
using namespace std;

SpillPass::SpillPass(FunctionAnalyzer &FA) : Pass(FA) {}

void SpillPass::run(Module &M) {
  SpilledValues = false;

  for (auto &F : M.functions()) {
    process(*F);
  }
}

void SpillPass::process(Function &F) {
  auto Coloring = FA.coloring(&F);
  for (auto &BB : F.basicBlocks()) {
    auto &Instructions = BB->instructions();
    for (auto InstrIt = Instructions.begin(); InstrIt != Instructions.end(); ++InstrIt) {
      int ArgPos = 0;
      for (auto &Arg : InstrIt->get()->arguments()) {
        auto ColorIt = Coloring->find(Arg);
        if (ColorIt == Coloring->end())
          continue;
        auto Color = ColorIt->second;
        if (!FA.isRegisterSpilled(Color))
          continue;

        SpilledValues = true;

        int InstructionId = NameGen::genInstructionId();
        ValueRef StackSlot(ValueType::StackSlot, Color);
        auto LoadSpilledArgInstr = make_unique<Instruction>(InstructionType::Load, InstructionId, StackSlot);
        LoadSpilledArgInstr->DontSpill = true;
        auto ArgPtr = LoadSpilledArgInstr.get();
        InstrIt = Instructions.insert(InstrIt, move(LoadSpilledArgInstr));
        ++InstrIt;

        InstrIt->get()->updateArg(ArgPos, ArgPtr);
        ++ArgPos;
      }

      auto ColorIt = Coloring->find(InstrIt->get());
      if (ColorIt == Coloring->end())
        continue;
      auto Color = ColorIt->second;
      if (!FA.isRegisterSpilled(Color))
        continue;

      SpilledValues = true;

      InstrIt->get()->DontSpill = true;

      int InstructionId = NameGen::genInstructionId();
      ValueRef StackSlot(ValueType::StackSlot, Color);
      auto StoreSpillInstr = make_unique<Instruction>(InstructionType::Store, InstructionId, InstrIt->get(), StackSlot);
      ++InstrIt;
      InstrIt = Instructions.insert(InstrIt, move(StoreSpillInstr));
    }
  }
}
