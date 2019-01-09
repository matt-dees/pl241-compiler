#ifndef CS241C_IR_GLOBAL_VARIABLE_H
#define CS241C_IR_GLOBAL_VARIABLE_H

#include "Value.h"
#include <string>
#include <vector>

namespace cs241c {
class GlobalVariable : public Value {
public:
  int WordCount;

  explicit GlobalVariable(std::string Ident);
  GlobalVariable(std::string Ident, const std::vector<int> &ArrayDimensions);
};
} // namespace cs241c

#endif
