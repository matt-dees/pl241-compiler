#ifndef CS241C_IR_DEADCODEELIMINATION_PASS
#define CS241C_IR_DEADCODEELIMINATION_PASS

#include "Pass.h"

namespace cs241c {
class Function;
class Module;

class DeadCodeEliminationPass : Pass {
public:
  void run(Module &) override;

private:
  void run(Function &F);
};
} // namespace cs241c

#endif
