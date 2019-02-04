#ifndef CS241C_IR_DEADCODEELIMINATION_PASS
#define CS241C_IR_DEADCODEELIMINATION_PASS

namespace cs241c {
class Function;
class Module;

class DeadCodeEliminationPass {
public:
  void run(Module &);

private:
  void run(Function &F);
};
} // namespace cs241c

#endif
