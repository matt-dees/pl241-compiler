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

  for (auto &BBPtr : F.basicBlocks()) {
    auto BB = BBPtr.get();

    auto &Predecessors = BB->predecessors();

    auto &Instructions = BB->instructions();
    auto InstructionsEnd = Instructions.end();

    // 1. First push all the move instructions to the parents.
    for (auto InstrIt = Instructions.begin(); InstrIt != InstructionsEnd; ++InstrIt) {
      auto &Instr = *InstrIt;

      // Phi instructions are only in the very beginning of the block, so it is safe to break once a non phi instruction
      // is encountered.
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

          Predecessors[I]->appendInstruction(move(Move));
        }
      }
    }
  }

  // 2. Update all arguments and remove phi instructions.
  for (auto &BBPtr : F.basicBlocks()) {
    auto &Instructions = BBPtr->instructions();
    for (auto &Instr : Instructions) {
      Instr->updateArgs(PhiSubstitutions);
    }

    Instructions.erase(remove_if(Instructions.begin(), Instructions.end(),
                                 [](const auto &Instr) { return Instr->InstrT == InstructionType::Phi; }),
                       Instructions.end());
  }
}

void Phi2VarPass::run(Module &M) {
  for (auto &F : M.functions()) {
    process(*F);
  }
}
