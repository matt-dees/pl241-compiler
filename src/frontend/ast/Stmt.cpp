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
  Ctx.currentBlock()->terminate(
      std::make_unique<RetInstruction>(Ctx.genInstructionId(), E->genIr(Ctx)));
}

FunctionCallStmt::FunctionCallStmt(FunctionCall CallExpr)
    : CallExpr(std::move(CallExpr)) {}

void FunctionCallStmt::genIr(IrGenContext &Ctx) const { CallExpr.genIr(Ctx); }

IfStmt::IfStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Then,
               std::vector<std::unique_ptr<Stmt>> Else)
    : Rel(std::move(Rel)), Then(move(Then)), Else(move(Else)) {}

void IfStmt::genIr(IrGenContext &Ctx) const {
  auto TrueBB = Ctx.makeBasicBlock();
  auto FalseBB = Ctx.makeBasicBlock();
  auto FollowBB = Ctx.makeBasicBlock();

  auto Comparison = Rel.genCmp(Ctx);
  auto Branch = Rel.genBranch(Ctx, Comparison, TrueBB, FalseBB);
  Ctx.currentBlock()->terminate(move(Branch));

  Ctx.currentBlock() = TrueBB;
  for (const std::unique_ptr<Stmt> &S : Then) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->terminate(
        std::make_unique<BraInstruction>(Ctx.genInstructionId(), FollowBB));
  }

  Ctx.currentBlock() = FalseBB;
  for (const std::unique_ptr<Stmt> &S : Then) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->terminate(
        std::make_unique<BraInstruction>(Ctx.genInstructionId(), FollowBB));
  }

  Ctx.currentBlock() = FollowBB;
}

WhileStmt::WhileStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Body)
    : Rel(std::move(Rel)), Body(std::move(Body)) {}

void WhileStmt::genIr(IrGenContext &) const {}
