#ifndef CS241C_IR_COMMONSUBEXELIMPASS_H
#define CS241C_IR_COMMONSUBEXELIMPASS_H

#include "Function.h"
#include "Module.h"
#include "Pass.h"
namespace cs241c {
class CommonSubexElimPass : public Pass {
public:
  void run(Module &) override;

private:
  void run(Function &F);

  bool shouldIgnore(Instruction *I);
  bool isPatchInst(Instruction *I);
};
} // namespace cs241c

#endif
