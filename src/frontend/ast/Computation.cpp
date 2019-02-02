#include "Computation.h"
#include "IrGenContext.h"
#include <algorithm>

using namespace cs241c;
using namespace std;

Computation::Computation(vector<unique_ptr<Decl>> Vars, vector<Func> Funcs) : Vars(move(Vars)), Funcs(move(Funcs)) {}

unique_ptr<Module> Computation::genIr() {
  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext Ctx(CompilationUnit.get());

  for (auto &Var : Vars) {
    Var->declareGlobal(Ctx);
  }

  for (auto &Function : Funcs) {
    Function.genIr(Ctx);
  }

  CompilationUnit->buildDominatorTree();
  Ctx.compUnitToSSA();
  return CompilationUnit;
}
