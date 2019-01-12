#include "Decl.h"
#include "Function.h"
#include "IrGenContext.h"

using namespace cs241c;

IntDecl::IntDecl(std::string Ident) : Ident(move(Ident)) {}

std::unique_ptr<GlobalVariable> IntDecl::genIr(IrGenContext &Ctx) {
  auto var = std::make_unique<GlobalVariable>(Ident);
  Ctx.declareGlobal(var.get());
  return var;
}

ArrayDecl::ArrayDecl(std::string Ident, std::vector<int32_t> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {}

std::unique_ptr<GlobalVariable> ArrayDecl::genIr(IrGenContext &Ctx) {
  auto var = std::make_unique<GlobalVariable>(Ident, Dim);
  Ctx.declareGlobal(var.get());
  return var;
}

Func::Func(Func::Type T, std::string Ident, std::vector<std::string> Params,
           std::vector<std::unique_ptr<Decl>> Vars,
           std::vector<std::unique_ptr<Stmt>> Stmts)
    : T(T), Ident(move(Ident)), Params(move(Params)), Vars(move(Vars)),
      Stmts(move(Stmts)) {}

std::unique_ptr<Function> Func::genIr(IrGenContext &Ctx) {
  std::unique_ptr<BasicBlock> EntryBlock = std::make_unique<BasicBlock>(0);
  Ctx.CurrentBlock = EntryBlock.get();

  for (const std::unique_ptr<Stmt> &S : Stmts) {
    S->genIr(Ctx);
  }

  std::vector<std::unique_ptr<BasicBlock>> Blocks;
  Blocks.push_back(move(EntryBlock));
  return std::make_unique<Function>(Ident, move(Blocks));
}
