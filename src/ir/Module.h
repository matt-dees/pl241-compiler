#ifndef CS241C_IR_MODULE_H
#define CS241C_IR_MODULE_H

#include "Function.h"
#include "FunctionAnalyzer.h"
#include "IrGenContext.h"
#include "Value.h"
#include "Variable.h"
#include "Vcg.h"
#include <memory>
#include <vector>

namespace cs241c {
class Module : public Vcg {
  NamedValue GlobalBase{"GlobalBase"};
  const std::string Name;

public:
  std::vector<std::unique_ptr<GlobalVariable>> Globals;
  std::vector<std::unique_ptr<Function>> Functions;

public:
  Module(std::string ModuleName);
  Value *globalBase();
  std::string getIdentifier() const;

  std::vector<std::unique_ptr<GlobalVariable>> &globals();
  const std::vector<std::unique_ptr<GlobalVariable>> &globals() const;
  std::vector<std::unique_ptr<Function>> &functions();
  const std::vector<std::unique_ptr<Function>> &functions() const;

protected:
  void writeBasicBlock(std::ofstream &OutFileStream, BasicBlock *BB,
                       const std::string &Title);
  void writeEdge(std::ofstream &OutFileStream, BasicBlock *Source,
                 BasicBlock *Destination);
  void writeFunction(std::ofstream &OutFileStream, Function *F);
  void writeGraph(std::ofstream &OutFileStream) override;
};
} // namespace cs241c

#endif
