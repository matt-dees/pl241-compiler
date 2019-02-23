#include "Phi2VarPass.h"
#include "FunctionAnalyzer.h"
#include "Module.h"
#include "NameGen.h"
#include "RegisterAllocator.h"
#include <algorithm>
#include <map>

using namespace cs241c;
using namespace std;

void Phi2VarPass::process(Function &F) {
  auto Registers = FA.coloring(&F);
  map<ValueRef, ValueRef> PhiSubstitutions;

  auto Blocks = F.postOrderCfg();
  reverse(Blocks.begin(), Blocks.end());
  for (auto BB : Blocks) {
    auto &Predecessors = BB->predecessors();

    auto &Instructions = BB->instructions();
    auto InstructionsEnd = Instructions.end();
    auto InstrIt = Instructions.begin();
    for (; InstrIt != InstructionsEnd; ++InstrIt) {
      auto &Instr = *InstrIt;

      if (Instr->InstrT != InstructionType::Phi)
        break;

      auto TargetRegister =
          Registers->find(Instr.get()) != Registers->end() ? Registers->at(Instr.get()) : RegisterAllocator::SPILL;
      ValueRef Target = TargetRegister != RegisterAllocator::RA_REGISTER::SPILL
                            ? ValueRef(ValueType::Register, TargetRegister)
                            : Instr->storage();
      PhiSubstitutions[Instr.get()] = Target;

      auto Args = Instr->arguments();
      for (int I = 0; I < 2; ++I) {
        auto Arg = Args[I];
        auto ArgRegister = Registers->find(Arg) != Registers->end() ? Registers->at(Arg) : RegisterAllocator::SPILL;
        if (ArgRegister != TargetRegister) {
          ValueRef Source =
              ArgRegister != RegisterAllocator::RA_REGISTER::SPILL ? ValueRef(ValueType::Register, ArgRegister) : Arg;
          auto Move = make_unique<MoveInstruction>(NameGen::genInstructionId(), Source, Target);
          Move->storage() = Instr->storage();

          auto TargetPredecessor = Predecessors[I];
          if (TargetPredecessor->successors().size() > 1) {
            auto IntermediateBB = make_unique<BasicBlock>(NameGen::genBasicBlockName());
            TargetPredecessor->updateSuccessor(BB, IntermediateBB.get());
            IntermediateBB->fallthoughSuccessor() = BB;
            TargetPredecessor = IntermediateBB.get();
            F.basicBlocks().push_back(move(IntermediateBB));
          }

          Predecessors[I]->appendInstruction(move(Move));
        }
      }
    }
    Instructions.erase(Instructions.begin(), InstrIt);

    for (auto &Instr : Instructions) {
      Instr->updateArgs(PhiSubstitutions);
    }
  }
}

void Phi2VarPass::run(Module &M) {
  for (auto &F : M.functions()) {
    process(*F);
  }
}
