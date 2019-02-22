#ifndef CS241C_IR_MEM2VARPASS_H
#define CS241C_IR_MEM2VARPASS_H

#include "Pass.h"

namespace cs241c {
class Mem2VarPass : public Pass {
public:
  Mem2VarPass(FunctionAnalyzer &FA) : Pass(FA) {}
  void run(Module &) override;
};
} // namespace cs241c

#endif
