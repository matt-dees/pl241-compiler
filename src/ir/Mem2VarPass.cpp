#include "Mem2VarPass.h"
#include "Module.h"
#include <algorithm>

using namespace cs241c;
using namespace std;

void Mem2VarPass::run(Module *M) {
  for (auto &F : M->functions()) {
    run(F.get());
  }
}

void Mem2VarPass::run(Function *F) {
  CurrentFunction = F;
  BasicBlock *NextBB = F->entryBlock();
  while (NextBB) {
    NextBB = run(NextBB);
  }
}

BasicBlock *Mem2VarPass::run(BasicBlock *BB) {
  const auto BBEnd = BB->end();
  for (auto Instr = BB->begin(); Instr != BBEnd; ++Instr) {
    Instruction *InstrPtr = Instr->get();
    if (auto Load = dynamic_cast<LoadInstruction *>(InstrPtr)) {
      auto Object = Load->object();
      FunctionLoads.insert(Object);
      if (!Object->isSingleWord())
        continue;
      auto KnownVar = KnownVars.find(Object);
      if (KnownVar != KnownVars.end()) {
        LocalVariable *KnownVarPtr = KnownVar->second;
        for_each(Instr + 1, BBEnd, [InstrPtr, KnownVarPtr](unique_ptr<Instruction> &I) {
          replace(I->arguments().begin(), I->arguments().end(), static_cast<Value *>(InstrPtr),
                  static_cast<Value *>(KnownVarPtr));
        });
      }
    } else if (auto Store = dynamic_cast<StoreInstruction *>(InstrPtr)) {
    } else if (auto Call = dynamic_cast<CallInstruction *>(InstrPtr)) {
    }
  }
  return nullptr;
}
