#ifndef CS241C_IR_SPILLPASS_H
#define CS241C_IR_SPILLPASS_H

#include "Pass.h"

namespace cs241c {
class Function;

class SpillPass : public Pass {
public:
  SpillPass(FunctionAnalyzer &);
  void run(Module &) override;

  bool SpilledValues;

private:
  void process(Function &);
};
} // namespace cs241c

#endif
