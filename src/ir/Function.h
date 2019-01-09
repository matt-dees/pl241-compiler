#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include <memory>

namespace cs241c {
class Function {
public:
  explicit Function(const std::string &Name);
  const std::vector<BasicBlock> BasicBlocks;
  const std::string Name;
};
} // namespace cs241c

#endif
