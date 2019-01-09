#ifndef CS241C_IR_MODULE_H
#define CS241C_IR_MODULE_H

#include "Function.h"
#include <vector>

namespace cs241c {
class Module {
public:
  explicit Module(const std::string &ModuleName,
                  std::vector<std::unique_ptr<Function>> Functions = {});
  std::vector<std::unique_ptr<Function>> Functions;

  const std::string getIdentifier() const;

private:
  const std::string Name;
};
} // namespace cs241c

#endif
