#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class Function {
public:
  explicit Function(const std::string &Name,
                    std::vector<std::unique_ptr<BasicBlock>> BasicBlocks);
  const std::vector<std::unique_ptr<BasicBlock>> BasicBlocks;
  const std::string Name;
};
} // namespace cs241c

#endif
