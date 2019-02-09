#ifndef CS241C_IR_INTEGRITYCHECKPASS_H
#define CS241C_IR_INTEGRITYCHECKPASS_H

#include "Pass.h"

namespace cs241c {
class IntegrityCheckPass : public Pass {
public:
  void run(Module &) override;
};
} // namespace cs241c

#endif
