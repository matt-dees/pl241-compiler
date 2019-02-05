#include "Mem2VarPass.h"
#include "Module.h"
#include "NameGen.h"
#include <algorithm>

using namespace cs241c;
using namespace std;

void Mem2VarPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(F.get());
  }
}

void Mem2VarPass::run(Function *F) {
  CurrentFunction = F;
  CurrentFunctionLoads.clear();
  CurrentFunctionStores.clear();
  KnownVars.clear();

  BasicBlock *NextBB = F->entryBlock();
  while (NextBB) {
    NextBB = run(NextBB);
  }
}

BasicBlock *Mem2VarPass::run(BasicBlock *BB) {
  const auto BBEnd = BB->end();
  for (size_t InstrI = 0; InstrI < BB->instructions().size(); ++InstrI) {
    Instruction *InstrPtr = BB->instructions()[InstrI].get();
    if (auto Load = dynamic_cast<LoadInstruction *>(InstrPtr)) {
      auto Object = Load->object();
      CurrentFunctionLoads.insert(Object);
      if (!Object->isSingleWord())
        continue;
      auto KnownVar = KnownVars.find(Object);
      if (KnownVar != KnownVars.end()) {
        LocalVariable *KnownVarPtr = KnownVar->second;
        for_each(BB->begin() + InstrI + 1, BBEnd, [InstrPtr, KnownVarPtr](unique_ptr<Instruction> &I) {
          replace(I->arguments().begin(), I->arguments().end(), static_cast<Value *>(InstrPtr),
                  static_cast<Value *>(KnownVarPtr));
        });
      } else {
        auto Local = make_unique<LocalVariable>(string("$") + Object->ident());
        auto LocalPtr = Local.get();
        auto Move = make_unique<MoveInstruction>(NameGen::genInstructionId(), Load, Local.get());
        CurrentFunction->locals().push_back(move(Local));
        BB->instructions().insert(BB->begin() + InstrI + 1, move(Move));
        KnownVars[Object] = LocalPtr;
      }
    } else if (auto Store = dynamic_cast<StoreInstruction *>(InstrPtr)) {
    } else if (auto Call = dynamic_cast<CallInstruction *>(InstrPtr)) {
    }
  }
  return nullptr;
}
