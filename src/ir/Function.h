#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class Function {
public:
  explicit Function(std::string Name,
                    std::vector<std::unique_ptr<BasicBlock>> BasicBlocks = {});
  std::vector<std::unique_ptr<BasicBlock>> BasicBlocks;
  std::string Name;
};
} // namespace cs241c

#endif
