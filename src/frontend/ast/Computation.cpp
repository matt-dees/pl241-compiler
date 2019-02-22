#include "Computation.h"
#include "IrGenContext.h"
#include <algorithm>

using namespace cs241c;
using namespace std;

Computation::Computation(vector<unique_ptr<Decl>> Vars, vector<Func> Funcs)
    : Vars(move(Vars)), Funcs(move(Funcs)) {}

unique_ptr<Module> Computation::genIr() {
  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext IrCtx(CompilationUnit.get());

  for (auto &Var : Vars) {
    Var->declareGlobal(IrCtx);
  }

  for (auto &Function : Funcs) {
    Function.genIr(IrCtx);
  }

  return CompilationUnit;
}
