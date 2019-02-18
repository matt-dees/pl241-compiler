#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include "DominatorTree.h"
#include "IrGenContext.h"
#include "RegisterAllocator.h"
#include "SSAContext.h"
#include "Value.h"
#include "Variable.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class BasicBlock;

class Function : public Value {
  std::string Name;
  std::vector<std::unique_ptr<ConstantValue>> Constants;
  std::vector<std::unique_ptr<LocalVariable>> Locals;
  std::vector<std::unique_ptr<BasicBlock>> BasicBlocks;

  std::unordered_map<Value *, std::unique_ptr<RegAllocValue>>
      ValueToRegAllocVal;
  DominatorTree DT;
  InterferenceGraph IG;

private:
  SSAContext recursiveNodeToSSA(BasicBlock *CurrentBB, SSAContext Ctx);
  void recursiveGenAllPhis(BasicBlock *CurrentBB, IrGenContext &GenCtx);
  void propagateChangeToPhis(BasicBlock *SourceBB, Variable *ChangedVar,
                             Value *NewVal);
  RegAllocValue *lookupRegAllocVal(Value *);

public:
  Function() = default;
  Function(std::string Name,
           std::vector<std::unique_ptr<LocalVariable>> &&Locals);

  void buildInterferenceGraph();
  void buildDominatorTree();
  void toSSA(IrGenContext &GenCtx);

  DominatorTree &dominatorTree();
  InterferenceGraph &interferenceGraph() { return IG; }
  std::vector<std::unique_ptr<ConstantValue>> &constants();
  std::vector<std::unique_ptr<LocalVariable>> &locals();
  BasicBlock *entryBlock() const;
  std::vector<std::unique_ptr<BasicBlock>> &basicBlocks();
  const std::vector<std::unique_ptr<BasicBlock>> &basicBlocks() const;
  std::string toString() const override;

  std::vector<BasicBlock *> postOrderCfg();
};
} // namespace cs241c

#endif
