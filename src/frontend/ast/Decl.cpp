#include "Decl.h"
#include "Function.h"
#include "IrGenContext.h"
#include "Variable.h"
#include <algorithm>

using namespace cs241c;

IntDecl::IntDecl(std::string Ident) : Ident(move(Ident)) {}

std::unique_ptr<GlobalVariable> IntDecl::declareGlobal(IrGenContext &Ctx) {
  auto var = std::make_unique<GlobalVariable>(Ident);
  Ctx.declare(var.get());
  return var;
}

std::unique_ptr<LocalVariable> IntDecl::declareLocal(IrGenContext &Ctx) {
  auto var = std::make_unique<LocalVariable>(Ident);
  Ctx.declare(var.get());
  return var;
}

ArrayDecl::ArrayDecl(std::string Ident, std::vector<int32_t> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {}

std::unique_ptr<GlobalVariable> ArrayDecl::declareGlobal(IrGenContext &Ctx) {
  auto var = std::make_unique<GlobalVariable>(Ident, Dim);
  Ctx.declare(var.get());
  return var;
}

std::unique_ptr<LocalVariable> ArrayDecl::declareLocal(IrGenContext &Ctx) {
  auto var = std::make_unique<LocalVariable>(Ident, Dim);
  Ctx.declare(var.get());
  return var;
}

Func::Func(Func::Type T, std::string Ident, std::vector<std::string> Params,
           std::vector<std::unique_ptr<Decl>> Vars,
           std::vector<std::unique_ptr<Stmt>> Stmts)
    : T(T), Ident(move(Ident)), Params(move(Params)), Vars(move(Vars)),
      Stmts(move(Stmts)) {}

std::unique_ptr<Function> Func::genIr(IrGenContext &Ctx) {
  Ctx.beginScope();

  std::vector<std::unique_ptr<LocalVariable>> Locals;
  std::transform(Vars.begin(), Vars.end(), back_inserter(Locals),
                 [&Ctx](const std::unique_ptr<Decl> &Var) {
                   return Var->declareLocal(Ctx);
                 });

  BasicBlock *EntryBlock = Ctx.makeBasicBlock();
  Ctx.currentBlock() = EntryBlock;

  for (const std::unique_ptr<Stmt> &S : Stmts) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->terminate(
        std::make_unique<RetInstruction>(Ctx.genInstructionId()));
  }

  auto Func = std::make_unique<Function>(Ident, move(Ctx.constants()),
                                         move(Locals), Ctx.blocks());
  Ctx.declare(Func.get());
  return Func;
}
