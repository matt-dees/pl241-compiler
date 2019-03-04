#ifndef CS241C_IR_SSAPASS_H
#define CS241C_IR_SSAPASS_H

#include "BasicBlock.h"
#include "FunctionAnalyzer.h"
#include "Module.h"
#include "Pass.h"
#include "SSAContext.h"

namespace cs241c {
class SSAPass : public Pass {
public:
  SSAPass(FunctionAnalyzer &FA) : Pass(FA) {}
  void run(Module &) override;

private:
  Function *CurrentFunction;

  void run(Function &);
  SSAContext recursiveNodeToSSA(BasicBlock *CurrentBB, SSAContext Ctx, std::unordered_set<BasicBlock *> &Visited);
  void recursiveGenAllPhis(BasicBlock *CurrentBB, std::unordered_set<BasicBlock *> &Visited);
  void propagateChangeToPhis(BasicBlock *SourceBB, Variable *ChangedVar, Value *NewVal);
  void basicBlockToSSA(BasicBlock &BB, SSAContext &SSCtx);
};
} // namespace cs241c

#endif