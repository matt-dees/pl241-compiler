#ifndef CS241C_IR_CONSTEXPREVALPASS_H
#define CS241C_IR_CONSTEXPREVALPASS_H

#include "Pass.h"

namespace cs241c {
class ConstExprEvalPass : public Pass {
public:
  ConstExprEvalPass(FunctionAnalyzer &FA) : Pass(FA) {}
  void run(Module &) override;
};

} // namespace cs241c

#endif
