#ifndef CS241C_IR_BASICBLOCK_H
#define CS241C_IR_BASICBLOCK_H

#include "BasicBlockTerminator.h"
#include "Instruction.h"
#include <vector>

namespace cs241c {
class BasicBlock {
public:
  explicit BasicBlock();
  std::vector<Instruction *> Instructions;
  BasicBlockTerminator Terminator;
};
} // namespace cs241c

#endif
