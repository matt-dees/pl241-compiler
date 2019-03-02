#ifndef CS241C_IR_KILLPASS_H
#define CS241C_IR_KILLPASS_H

#include "Function.h"
#include "Pass.h"

namespace cs241c {
class KillPass : public Pass {
public:
  KillPass(FunctionAnalyzer &FA) : Pass(FA) {}
  void run(Module &) override;

private:
  void run(Function &F);
};
} // namespace cs241c

#endif
