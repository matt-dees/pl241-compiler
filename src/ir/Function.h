#ifndef CS241C_IR_FUNCTION_H
#define CS241C_IR_FUNCTION_H

#include "BasicBlock.h"
#include <memory>

namespace cs241c {
class Function {
  std::unique_ptr<BasicBlock> EntryBlock;
};
} // namespace cs241c

#endif
