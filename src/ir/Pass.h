#ifndef CS241C_IR_PASS_H
#define CS241C_IR_PASS_H

namespace cs241c {
class Module;
class FunctionAnalyzer;

class Pass {
public:
  virtual ~Pass() = default;
  Pass(FunctionAnalyzer &FA) : FA(FA) {}
  virtual void run(Module &) = 0;

protected:
  FunctionAnalyzer &FA;
};
} // namespace cs241c

#endif
