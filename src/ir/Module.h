#ifndef CS241C_IR_MODULE_H
#define CS241C_IR_MODULE_H

#include "DominatorTree.h"
#include "Function.h"
#include "Value.h"
#include "Variable.h"
#include <memory>
#include <vector>

namespace cs241c {
class Module {
  NamedValue GlobalBase{"GlobalBase"};
  const std::string Name;

private:
  SSAContext nodeToSSA(BasicBlock *CurrentBB, SSAContext SSACtx);

public:
  std::vector<std::unique_ptr<GlobalVariable>> Globals;
  std::vector<std::unique_ptr<Function>> Functions;
  DominatorTree DT;

public:
  Module(std::string ModuleName);
  void buildDominatorTree();
  Value *globalBase();
  std::string getIdentifier() const;
  void toSSA();
  std::vector<std::unique_ptr<GlobalVariable>> &globals();
  const std::vector<std::unique_ptr<GlobalVariable>> &globals() const;
  std::vector<std::unique_ptr<Function>> &functions();
  const std::vector<std::unique_ptr<Function>> &functions() const;
};
} // namespace cs241c

#endif
