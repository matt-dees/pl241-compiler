#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include "Value.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class BasicBlock;

class Function : public Value {
public:
  std::string Name;
  std::vector<std::unique_ptr<BasicBlock>> BasicBlocks;

  Function() = default;
  Function(std::string Name,
           std::vector<std::unique_ptr<BasicBlock>> &&BasicBlocks);

  std::string toString() const override;
};
} // namespace cs241c

#endif
