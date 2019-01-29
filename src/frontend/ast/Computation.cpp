#include "Computation.h"
#include "IrGenContext.h"
#include <algorithm>

using namespace cs241c;

Computation::Computation(std::vector<std::unique_ptr<Decl>> Vars,
                         std::vector<Func> Funcs)
    : Vars(move(Vars)), Funcs(move(Funcs)) {}

std::unique_ptr<Module> Computation::genIr() {
  auto CompilationUnit = std::make_unique<Module>("program");
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
