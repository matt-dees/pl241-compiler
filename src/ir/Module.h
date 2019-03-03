#ifndef CS241C_IR_MODULE_H
#define CS241C_IR_MODULE_H

#include "Function.h"
#include "FunctionAnalyzer.h"
#include "Vcg.h"
#include <memory>
#include <vector>

namespace cs241c {
class FunctionAnalyzer;

class Module : public Vcg {
  NamedValue GlobalBase{"GlobalBase"};
  const std::string Name;

public:
  std::vector<std::unique_ptr<GlobalVariable>> Globals;
  std::vector<std::unique_ptr<Function>> Functions;

  // I want to print with registers...
  FunctionAnalyzer *FA = nullptr;

public:
  Module(std::string ModuleName);
  Value *globalBase();
  std::string getIdentifier() const;

  std::vector<std::unique_ptr<GlobalVariable>> &globals();
  const std::vector<std::unique_ptr<GlobalVariable>> &globals() const;
  std::vector<std::unique_ptr<Function>> &functions();
  const std::vector<std::unique_ptr<Function>> &functions() const;

private:
  void writeBasicBlock(std::ofstream &OutFileStream, BasicBlock *BB, const std::string &Title, Function *F);
  void writeEdge(std::ofstream &OutFileStream, BasicBlock *Source, BasicBlock *Destination);
  void writeFunction(std::ofstream &OutFileStream, Function *F);
  void writeGraph(std::ofstream &OutFileStream) override;
};
} // namespace cs241c

#endif
