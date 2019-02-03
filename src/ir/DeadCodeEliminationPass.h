#ifndef CS241C_IR_DEADCODEELIMINATION_PASS
#define CS241C_IR_DEADCODEELIMINATION_PASS

namespace cs241c {
class Module;

class DeadCodeEliminationPass {
public:
  void run(Module &);
};
} // namespace cs241c

#endif
