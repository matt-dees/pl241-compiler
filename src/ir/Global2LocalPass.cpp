#include "Global2LocalPass.h"
#include "Module.h"

using namespace cs241c;

namespace {
class FunctionEliminator {
  Function *F;

public:
  void run(Function *F) {
    this->F = F;
    BasicBlock *NextBB = F->entryBlock();
    while (NextBB) {
      NextBB = run(NextBB);
    }
  }

private:
  BasicBlock *run(BasicBlock *BB) {
    auto BBEnd = BB->end();
    for (auto Instr = BB->begin(); Instr != BBEnd; ++Instr) {
    }
    return nullptr;
  }
};
} // namespace

void Global2LocalPass::run(Module *M) {
  for (auto &F : M->functions()) {
    FunctionEliminator Eliminator;
    Eliminator.run(F.get());
  }
}
