#include "ParamPass.h"
#include "Module.h"
#include "NameGen.h"
#include <unordered_map>

using namespace cs241c;
using namespace std;

ParamPass ::ParamPass(FunctionAnalyzer &FA) : Pass(FA) {}

void ParamPass::run(Module &M) {
  for (auto &F : M.functions()) {
    process(*F);
  }
}

void ParamPass::process(Function &F) {
  auto Parameters = F.parameters();
  unordered_map<Value *, int> ParamSlots;

  int Slot = 0;
  for (auto Parameter : Parameters) {
    ParamSlots[Parameter] = Slot;
    ++Slot;
  }

  for (auto &BB : F.basicBlocks()) {
    auto &Instructions = BB->instructions();
    for (auto InstrIt = Instructions.begin(); InstrIt != Instructions.end(); ++InstrIt) {
      auto &Instr = *InstrIt;

      auto &Args = Instr->arguments();
      for (int ArgPos = 0; ArgPos < Args.size(); ++ArgPos) {
        auto Arg = Args[ArgPos];

        if (ParamSlots.find(Arg) != ParamSlots.end()) {
          int InstructionId = NameGen::genInstructionId();
          ValueRef ParamSlot(ValueType::Parameter, ParamSlots[Arg]);
          auto LoadParameter = make_unique<Instruction>(InstructionType::LoadS, InstructionId, ParamSlot);
          LoadParameter->owner() = BB.get();
          LoadParameter->storage() = dynamic_cast<LocalVariable *>(Arg.Ptr);
          LoadParameter->DontSpill = true;
          auto ArgPtr = LoadParameter.get();
          InstrIt = Instructions.insert(InstrIt, move(LoadParameter));
          ++InstrIt;

          InstrIt->get()->updateArg(ArgPos, ArgPtr);
        }
      }
    }
  }
}
