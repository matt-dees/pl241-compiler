#include "Computation.h"
#include "IrGenContext.h"
#include <algorithm>

using namespace cs241c;

Computation::Computation(std::vector<std::unique_ptr<Decl>> Vars,
                         std::vector<Func> Funcs)
    : Vars(move(Vars)), Funcs(move(Funcs)) {}

Module Computation::genIr() {
  IrGenContext Ctx;

  std::vector<std::unique_ptr<GlobalVariable>> Globals;
  std::transform(
      Vars.begin(), Vars.end(), back_inserter(Globals),
      [&Ctx](const std::unique_ptr<Decl> &Var) { return Var->genIr(Ctx); });

  std::vector<std::unique_ptr<Function>> Functions;
  std::transform(Funcs.begin(), Funcs.end(), back_inserter(Functions),
                 [&Ctx](Func &F) { return F.genIr(Ctx); });

  return Module("program", move(Globals), move(Functions));
}
