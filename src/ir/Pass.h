#ifndef CS241C_IR_PASS_H
#define CS241C_IR_PASS_H

namespace cs241c {
class Module;

class Pass {
public:
  virtual ~Pass() = default;
  virtual void run(Module &) = 0;
};
} // namespace cs241c

#endif
