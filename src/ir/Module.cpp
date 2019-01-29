#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;

Module::Module(std::string ModuleName) : Name(move(ModuleName)) {}

Value *Module::globalBase() { return &GlobalBase; }
std::string Module::getIdentifier() const { return Name; }

std::vector<std::unique_ptr<GlobalVariable>> &Module::globals() {
  return Globals;
}

const std::vector<std::unique_ptr<GlobalVariable>> &Module::globals() const {
  return Globals;
}

std::vector<std::unique_ptr<Function>> &Module::functions() {
  return Functions;
}

const std::vector<std::unique_ptr<Function>> &Module::functions() const {
  return Functions;
}

void Module::buildDominatorTree() {
  if (Functions.empty()) {
    return;
  }
  Function *FuncStart = Functions.at(0).get();
  if (FuncStart->basicBlocks().empty()) {
    return;
  }
  DT.buildDominatorTree(FuncStart->basicBlocks().at(0).get());
}
