#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include <memory>

namespace cs241c {
class Function {
public:
  explicit Function(const std::string &Name);
  std::unique_ptr<BasicBlock> EntryBlock;
  const std::string Name;
};
} // namespace cs241c

#endif
