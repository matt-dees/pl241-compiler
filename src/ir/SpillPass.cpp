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
      if (InstrIt->get()->InstrT == InstructionType::Adda) {
      } else if (InstrIt->get()->InstrT == InstructionType::Phi) {
        auto PhiColorIt = Coloring->find(InstrIt->get());
        auto PhiColor = PhiColorIt->second;

        auto Args = InstrIt->get()->arguments();
        for (int ArgPos = 0; ArgPos < Args.size(); ++ArgPos) {
          auto Arg = Args[ArgPos];

          auto ArgColorIt = Coloring->find(Arg);
          if (ArgColorIt != Coloring->end()) {
            auto ArgColor = ArgColorIt->second;
            if (ArgColor == PhiColor)
              continue;

            if (FA.isRegisterSpilled(ArgColor)) {

              if (auto X = dynamic_cast<Instruction *>(Arg.Ptr)) {
                if (X->InstrT == InstructionType::Phi) {
                  int x = 0;
                }
              }

              auto SourceBlock = BB->predecessors()[ArgPos];

              int Id = NameGen::genInstructionId();
              ValueRef StackSlot(ValueType::StackSlot, ArgColor);
              auto LoadSpill = make_unique<Instruction>(InstructionType::LoadS, Id, StackSlot);
              auto LoadSpillPtr = LoadSpill.get();
              LoadSpill->owner() = SourceBlock;
              LoadSpill->storage() = InstrIt->get()->storage();
              LoadSpill->DontSpill = true;
              SourceBlock->appendInstruction(move(LoadSpill));

              InstrIt->get()->updateArg(ArgPos, LoadSpillPtr);
              Arg = LoadSpillPtr;
            }
          }

          if (FA.isRegisterSpilled(PhiColor)) {
            auto SourceBlock = BB->predecessors()[ArgPos];

            ValueRef StackSlot(ValueType::StackSlot, PhiColor);
            int Id = NameGen::genInstructionId();
            auto StoreSpill = make_unique<Instruction>(InstructionType::StoreS, Id, Arg, StackSlot);
            StoreSpill->owner() = SourceBlock;
            StoreSpill->storage() = InstrIt->get()->storage();
            StoreSpill->DontSpill = true;
            SourceBlock->appendInstruction(move(StoreSpill));
          }
        }
      } else {
        auto Args = InstrIt->get()->arguments();
        for (int ArgPos = 0; ArgPos < Args.size(); ++ArgPos) {
          auto Arg = Args[ArgPos];
          if (Arg.ValTy == ValueType::Value) {
            auto ArgInstr = dynamic_cast<Instruction *>(Arg.Ptr);
            if (ArgInstr && ArgInstr->InstrT == InstructionType::Adda) {
              continue;
            }
          }

          auto ColorIt = Coloring->find(Arg);
          if (ColorIt == Coloring->end())
            continue;
          auto Color = ColorIt->second;
          if (!FA.isRegisterSpilled(Color))
            continue;

          SpilledValues = true;

          int InstructionId = NameGen::genInstructionId();
          ValueRef StackSlot(ValueType::StackSlot, Color);
          auto LoadSpilledArgInstr = make_unique<Instruction>(InstructionType::LoadS, InstructionId, StackSlot);
          LoadSpilledArgInstr->owner() = BB.get();
          LoadSpilledArgInstr->storage() = InstrIt->get()->storage();
          LoadSpilledArgInstr->DontSpill = true;
          auto ArgPtr = LoadSpilledArgInstr.get();
          InstrIt = Instructions.insert(InstrIt, move(LoadSpilledArgInstr));
          ++InstrIt;

          InstrIt->get()->updateArg(ArgPos, ArgPtr);
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
        auto StoreSpillInstr =
            make_unique<Instruction>(InstructionType::StoreS, InstructionId, InstrIt->get(), StackSlot);
        StoreSpillInstr->owner() = BB.get();
        StoreSpillInstr->storage() = InstrIt->get()->storage();
        StoreSpillInstr->DontSpill = true;
        ++InstrIt;
        InstrIt = Instructions.insert(InstrIt, move(StoreSpillInstr));
      }
    }
  }

  for (auto &BB : F.basicBlocks()) {
    auto &Instructions = BB->instructions();
    Instructions.erase(remove_if(Instructions.begin(), Instructions.end(),
                                 [this, Coloring](auto &Instr) {
                                   if (Instr->InstrT == InstructionType::Phi) {
                                     auto PhiColorIt = Coloring->find(Instr.get());
                                     auto PhiColor = PhiColorIt->second;
                                     if (FA.isRegisterSpilled(PhiColor)) {
                                       return true;
                                     }
                                   }
                                   return false;
                                 }),
                       Instructions.end());
  }
}
