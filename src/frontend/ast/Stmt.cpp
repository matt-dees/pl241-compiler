#include "Stmt.h"
#include "IrGenContext.h"

using namespace cs241c;

Assignment::Assignment(std::unique_ptr<Designator> Lhs,
                       std::unique_ptr<Expr> Rhs)
    : Lhs(move(Lhs)), Rhs(move(Rhs)) {}

void Assignment::genIr(IrGenContext &Ctx) const {
  Lhs->genStore(Ctx, Rhs->genIr(Ctx));
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> E) : E(move(E)) {}

void ReturnStmt::genIr(IrGenContext &Ctx) const {
  Ctx.makeInstruction<RetInstruction>(E->genIr(Ctx));
}

FunctionCallStmt::FunctionCallStmt(FunctionCall CallExpr)
    : CallExpr(std::move(CallExpr)) {}

void FunctionCallStmt::genIr(IrGenContext &Ctx) const { CallExpr.genIr(Ctx); }

IfStmt::IfStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Then,
               std::vector<std::unique_ptr<Stmt>> Else)
    : Rel(std::move(Rel)), Then(move(Then)), Else(move(Else)) {}

void IfStmt::genIr(IrGenContext &) const {}

WhileStmt::WhileStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Body)
    : Rel(std::move(Rel)), Body(std::move(Body)) {}

void WhileStmt::genIr(IrGenContext &) const {}
