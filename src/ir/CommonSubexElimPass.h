#ifndef CS241C_IR_COMMONSUBEXELIMPASS_H
#define CS241C_IR_COMMONSUBEXELIMPASS_H

#include "Pass.h"

namespace cs241c {
class CommonSubexElimPass : public Pass {
public:
  void run(Module &) override;
};
} // namespace cs241c

#endif
