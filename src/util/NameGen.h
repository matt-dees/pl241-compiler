#ifndef CS241C_UTIL_NAMEGEN_H
#define CS241C_UTIL_NAMEGEN_H

#include <string>

namespace cs241c {
class NameGen {
  static int BasicBlockCounter;
  static int InstructionCounter;

public:
  NameGen() = delete;

  static std::string genBasicBlockName();
  static int genInstructionId();
};
} // namespace cs241c

#endif
