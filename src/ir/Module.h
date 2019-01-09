#ifndef CS241C_IR_MODULE_H
#define CS241C_IR_MODULE_H

#include "Function.h"
#include "GlobalVariable.h"
#include <memory>
#include <vector>

namespace cs241c {
class Module {
  const std::string Name;

public:
  std::vector<std::unique_ptr<GlobalVariable>> Globals;
  std::vector<std::unique_ptr<Function>> Functions;

  Module(std::string ModuleName,
         std::vector<std::unique_ptr<GlobalVariable>> &&Globals,
         std::vector<std::unique_ptr<Function>> &&Functions);
  const std::string getIdentifier() const;
};
} // namespace cs241c

#endif
