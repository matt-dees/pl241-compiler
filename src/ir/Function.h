#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include "DominatorTree.h"
#include "IrGenContext.h"
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
  DominatorTree DT;

private:
  SSAContext recursiveNodeToSSA(BasicBlock *CurrentBB, SSAContext Ctx);
  void recursiveGenAllPhis(BasicBlock *CurrentBB, IrGenContext &GenCtx);
  void propagateChangeToPhis(BasicBlock *SourceBB, Variable *ChangedVar,
                             Value *NewVal);

public:
  // Iterator for walking the CFG of this function from the bottom to the top
  class bottom_up_iterator_impl
      : public std::iterator<std::forward_iterator_tag, BasicBlock *> {

  public:
    friend class Function;

    bottom_up_iterator_impl(Function &Func)
        : CurrentIndex(0), BottomUpOrdering({}), Func(Func) {}
    unsigned long CurrentIndex;
    std::vector<BasicBlock *> BottomUpOrdering;
    Function &Func;

    bottom_up_iterator_impl begin();
    bottom_up_iterator_impl end();
    bottom_up_iterator_impl &operator++();
    bool operator==(const bottom_up_iterator_impl &Other) const;
    BasicBlock *&operator*();
    bool operator!=(bottom_up_iterator_impl Other);
  };

  using bottom_up_iterator = bottom_up_iterator_impl;

  Function() = default;
  Function(std::string Name,
           std::vector<std::unique_ptr<LocalVariable>> &&Locals);

  void buildDominatorTree();
  void toSSA(IrGenContext &GenCtx);
  void allocateRegisters();

  DominatorTree &dominatorTree();
  std::vector<std::unique_ptr<ConstantValue>> &constants();
  std::vector<std::unique_ptr<LocalVariable>> &locals();
  BasicBlock *entryBlock() const;
  std::vector<std::unique_ptr<BasicBlock>> &basicBlocks();
  const std::vector<std::unique_ptr<BasicBlock>> &basicBlocks() const;
  std::string toString() const override;
};
} // namespace cs241c

#endif
