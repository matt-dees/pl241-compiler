#ifndef CS241C_IR_MEM2VARPASS_H
#define CS241C_IR_MEM2VARPASS_H

#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace cs241c {
class BasicBlock;
class Function;
class GlobalVariable;
class LocalVariable;
class Module;
class Variable;

class Mem2VarPass {
  std::unordered_map<Function *, std::unordered_set<Variable *>> LoadsByFunction;
  std::unordered_map<Function *, std::unordered_set<Variable *>> StoresByFunction;

  Function *CurrentFunction;
  std::unordered_set<Variable *> CurrentFunctionLoads;
  std::unordered_set<Variable *> CurrentFunctionStores;
  std::unordered_map<Variable *, LocalVariable *> KnownVars;

public:
  void run(Module *);

private:
  void run(Function *F);
  BasicBlock *run(BasicBlock *BB);
};
} // namespace cs241c

#endif
