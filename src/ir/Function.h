#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
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

public:
  Function() = default;
  Function(std::string Name, std::vector<std::unique_ptr<LocalVariable>> &&Locals);

  std::vector<std::unique_ptr<ConstantValue>> &constants();
  std::vector<std::unique_ptr<LocalVariable>> &locals();
  BasicBlock *entryBlock() const;
  std::vector<std::unique_ptr<BasicBlock>> &basicBlocks();
  const std::vector<std::unique_ptr<BasicBlock>> &basicBlocks() const;
  std::string toString() const override;
};
} // namespace cs241c

#endif
