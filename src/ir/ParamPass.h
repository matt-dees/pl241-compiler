#ifndef CS241C_IR_PARAMPASS_H
#define CS241C_IR_PARAMPASS_H

#include "Pass.h"

namespace cs241c {
class Function;

class ParamPass : public Pass {
public:
  ParamPass(FunctionAnalyzer &);
  void run(Module &) override;

private:
  void process(Function &);
};
} // namespace cs241c

#endif
