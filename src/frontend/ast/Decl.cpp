#include "Decl.h"
#include "Function.h"
#include "IrGenContext.h"
#include "Variable.h"
#include <algorithm>

using namespace cs241c;

IntDecl::IntDecl(std::string Ident) : Ident(move(Ident)) {}

void IntDecl::declareGlobal(IrGenContext &Ctx) {
  auto Var = std::make_unique<GlobalVariable>(Ident);
  Symbol Sym{Var.get(), {}};
  Ctx.declare(Sym, move(Var));
}

std::unique_ptr<LocalVariable> IntDecl::declareLocal(IrGenContext &Ctx) {
  auto Var = std::make_unique<LocalVariable>(Ident);
  Symbol Sym{Var.get(), {}};
  Ctx.declare(Sym);
  Ctx.makeInstruction<MoveInstruction>(Ctx.makeConstant(0), Var.get());
  return Var;
}

ArrayDecl::ArrayDecl(std::string Ident, std::vector<int32_t> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {}

void ArrayDecl::declareGlobal(IrGenContext &Ctx) {
  auto Var = std::make_unique<GlobalVariable>(Ident, Dim);
  Symbol Sym{Var.get(), Dim};
  Ctx.declare(Sym, move(Var));
}

std::unique_ptr<LocalVariable> ArrayDecl::declareLocal(IrGenContext &Ctx) {
  auto Var = std::make_unique<LocalVariable>(Ident, Dim);
  Symbol Sym{Var.get(), Dim};
  Ctx.declare(Sym);
  return Var;
}

Func::Func(Func::Type T, std::string Ident, std::vector<std::string> Params,
           std::vector<std::unique_ptr<Decl>> Vars,
           std::vector<std::unique_ptr<Stmt>> Stmts)
    : T(T), Ident(move(Ident)), Params(move(Params)), Vars(move(Vars)),
      Stmts(move(Stmts)) {}

void Func::genIr(IrGenContext &Ctx) {
  Ctx.beginScope();

  BasicBlock *EntryBlock = Ctx.makeBasicBlock();
  Ctx.currentBlock() = EntryBlock;

  std::vector<std::unique_ptr<LocalVariable>> Locals;
  std::transform(Vars.begin(), Vars.end(), back_inserter(Locals),
                 [&Ctx](const std::unique_ptr<Decl> &Var) {
                   return Var->declareLocal(Ctx);
                 });

  for (const std::unique_ptr<Stmt> &S : Stmts) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->terminate(
        std::make_unique<RetInstruction>(Ctx.genInstructionId()));
  }

  auto Func = std::make_unique<Function>(Ident, move(Ctx.constants()),
                                         move(Locals), Ctx.blocks());
  Ctx.declare(move(Func));
}
