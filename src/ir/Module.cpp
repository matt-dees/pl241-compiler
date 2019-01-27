#include "Module.h"

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
