#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;
using namespace std;

Module::Module(string ModuleName) : Name(move(ModuleName)) {}

Value *Module::globalBase() { return &GlobalBase; }
string Module::getIdentifier() const { return Name; }

vector<unique_ptr<GlobalVariable>> &Module::globals() { return Globals; }

const vector<unique_ptr<GlobalVariable>> &Module::globals() const {
  return Globals;
}

vector<unique_ptr<Function>> &Module::functions() { return Functions; }

const vector<unique_ptr<Function>> &Module::functions() const {
  return Functions;
}

void Module::buildDominatorTree() {
  for (auto &F : functions()) {
    F->buildDominatorTree();
  }
}

void Module::toSSA(IrGenContext &Ctx) {
  for (auto &F : functions()) {
    F->toSSA(Ctx);
  }
}