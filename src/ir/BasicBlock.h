#ifndef CS241C_IR_BASICBLOCK_H
#define CS241C_IR_BASICBLOCK_H

#include "Instruction.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace cs241c {
class BasicBlock {
public:
  BasicBlock(uint32_t ID,
             std::vector<std::unique_ptr<Instruction>> Instructions = {});
  std::vector<std::unique_ptr<Instruction>> Instructions;
  BasicBlockTerminator Terminator;
  // Note: May want to put NextBlocks in BasicBlockTerminator class
  std::vector<BasicBlock *> NextBlocks;
  uint32_t ID;
};
} // namespace cs241c

#endif
