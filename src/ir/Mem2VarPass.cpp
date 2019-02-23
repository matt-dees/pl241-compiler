#include "Mem2VarPass.h"
#include "Module.h"
#include "NameGen.h"
#include <algorithm>
#include <map>
#include <stack>
#include <unordered_set>

using namespace cs241c;
using namespace std;

void Mem2VarPass::run(Module &M) {
  unordered_set<GlobalVariable *> ActuallyGlobal;

  for (auto &F : M.functions()) {
    if (F->name() == "main") {
      map<Variable *, LocalVariable *> LocalSubstitutions;

      auto InitConst = F->constant(0);

      for (auto &Global : M.globals()) {
        if (ActuallyGlobal.find(Global.get()) == ActuallyGlobal.end()) {
          auto Local = make_unique<LocalVariable>(string("_") + (Global->name().data() + 1), Global->wordCount());
          LocalSubstitutions[Global.get()] = Local.get();

          auto InitLocalMove = make_unique<MoveInstruction>(NameGen::genInstructionId(), InitConst, Local.get());
          InitLocalMove->owner() = F->entryBlock();

          auto &EntryInstructions = F->entryBlock()->instructions();
          EntryInstructions.insert(EntryInstructions.begin(), move(InitLocalMove));

          F->locals().push_back(move(Local));
        }
      }

      map<ValueRef, ValueRef> ArgSubstitutions;

      unordered_set<BasicBlock *> MarkedBlocks;
      stack<BasicBlock *> WorkingSet{{F->entryBlock()}};
      while (!WorkingSet.empty()) {
        BasicBlock *BB = WorkingSet.top();
        WorkingSet.pop();

        if (MarkedBlocks.find(BB) != MarkedBlocks.end()) {
          bool Change = false;
          for (auto &Instr : BB->instructions()) {
            Change |= Instr->updateArgs(ArgSubstitutions);
          }

          if (Change) {
            for (auto Successor : BB->successors()) {
              WorkingSet.push(Successor);
            }
          }
        } else {
          MarkedBlocks.insert(BB);

          auto &Instructions = BB->instructions();

          for (auto InstrIt = Instructions.begin(); InstrIt != Instructions.end(); ++InstrIt) {
            if (auto MemInstr = dynamic_cast<MemoryInstruction *>(InstrIt->get())) {
              if (LocalSubstitutions.find(MemInstr->object()) != LocalSubstitutions.end()) {
                if (MemInstr->InstrT == InstructionType::Load) {
                  ArgSubstitutions[MemInstr] = LocalSubstitutions[MemInstr->object()];
                } else if (MemInstr->InstrT == InstructionType::Store) {
                  auto Move = make_unique<MoveInstruction>(NameGen::genInstructionId(), MemInstr->arguments()[0],
                                                           LocalSubstitutions[MemInstr->object()]);
                  Move->owner() = BB;
                  InstrIt = Instructions.erase(InstrIt);
                  InstrIt = Instructions.insert(InstrIt, move(Move));
                }
              }
            }

            auto &Instr = *InstrIt;
            Instr->updateArgs(ArgSubstitutions);
            if (LocalSubstitutions.find(Instr->storage()) != LocalSubstitutions.end()) {
              Instr->storage() = LocalSubstitutions[Instr->storage()];
            }
          }

          for (auto Successor : BB->successors()) {
            WorkingSet.push(Successor);
          }
        }
      }
    } else {
      for (auto &BB : F->basicBlocks()) {
        for (auto &I : BB->instructions()) {
          for (ValueRef Arg : I->arguments()) {
            if (auto Global = dynamic_cast<GlobalVariable *>(Arg.R.Ptr)) {
              ActuallyGlobal.insert(Global);
            }
          }
        }
      }
    }
  }
}
