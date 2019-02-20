#ifndef CS241C_IR_PHI2VARPASS_H
#define CS241C_IR_PHI2VARPASS_H

#include "Pass.h"

namespace cs241c {
class Phi2VarPass : public Pass {
public:
  void run(Module &) override;
};
} // namespace cs241c

#endif
