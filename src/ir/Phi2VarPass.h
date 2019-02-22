#ifndef CS241C_IR_PHI2VARPASS_H
#define CS241C_IR_PHI2VARPASS_H

#include "Function.h"
#include "Pass.h"

namespace cs241c {
class Phi2VarPass : public Pass {
public:
  Phi2VarPass(FunctionAnalyzer &FA) : Pass(FA) {}
  void run(Module &) override;

protected:
  void process(Function &F);
};
} // namespace cs241c

#endif
