#ifndef CS241C_COMMONSUBEXELIMPASS_H
#define CS241C_COMMONSUBEXELIMPASS_H

#include "Function.h"
#include "Module.h"
#include "Pass.h"
namespace cs241c {
class CommonSubexElimPass : Pass {
public:
  void run(Module &) override;

private:
  void run(Function &F, const DominatorTree &DT);
};
} // namespace cs241c

#endif // CS241C_COMMONSUBEXELIMPASS_H
