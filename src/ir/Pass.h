#ifndef CS241C_PASS_H
#define CS241C_PASS_H

#include "Module.h"

namespace cs241c {
class Pass {
public:
  virtual void run(Module &) = 0;
};
} // namespace cs241c

#endif // CS241C_PASS_H
