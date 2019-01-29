#ifndef CS241C_IR_MODULE_H
#define CS241C_IR_MODULE_H

#include "DominatorTree.h"
#include "Function.h"
#include "IrGenContext.h"
#include "Variable.h"
#include <memory>
#include <vector>

namespace cs241c {
class Module {
  const std::string Name;

  void insertPhiInstructions(BasicBlock *BB, IrGenContext &Ctx);

public:
  std::vector<std::unique_ptr<GlobalVariable>> Globals;
  std::vector<std::unique_ptr<Function>> Functions;
  DominatorTree DT;

  Module(std::string ModuleName,
         std::vector<std::unique_ptr<GlobalVariable>> &&Globals,
         std::vector<std::unique_ptr<Function>> &&Functions);
  std::string getIdentifier() const;
  void toSSA(IrGenContext &Ctx);
  SSAContext cfgToSSA(BasicBlock *CurrentBB, SSAContext SSACtx);
};
} // namespace cs241c

#endif
