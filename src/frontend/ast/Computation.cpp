#include "Computation.h"
#include "IrGenContext.h"
#include "Mem2VarPass.h"
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

  // This should not be here, but we need to run it before generating SSA.
  Mem2VarPass M2V;
  M2V.run(*CompilationUnit);

  CompilationUnit->buildDominatorTree();
  CompilationUnit->toSSA(Ctx);
  return CompilationUnit;
}
