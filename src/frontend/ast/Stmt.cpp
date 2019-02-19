#include "Stmt.h"
#include "IrGenContext.h"

using namespace cs241c;
using namespace std;

Assignment::Assignment(unique_ptr<Designator> Lhs, unique_ptr<Expr> Rhs) : Lhs(move(Lhs)), Rhs(move(Rhs)) {}

void Assignment::genIr(IrGenContext &Ctx) const { Lhs->genStore(Ctx, Rhs->genIr(Ctx)); }

ReturnStmt::ReturnStmt(unique_ptr<Expr> E) : E(move(E)) {}

void ReturnStmt::genIr(IrGenContext &Ctx) const {
  Ctx.currentBlock()->terminate(
      make_unique<BasicBlockTerminator>(InstructionType::Ret, Ctx.genInstructionId(), vector<Value *>{E->genIr(Ctx)}));
}

FunctionCallStmt::FunctionCallStmt(FunctionCall CallExpr) : CallExpr(move(CallExpr)) {}

void FunctionCallStmt::genIr(IrGenContext &Ctx) const { CallExpr.genIr(Ctx); }

IfStmt::IfStmt(Relation Rel, vector<unique_ptr<Stmt>> Then, vector<unique_ptr<Stmt>> Else)
    : Rel(move(Rel)), Then(move(Then)), Else(move(Else)) {}

void IfStmt::genIr(IrGenContext &Ctx) const {
  auto TrueBB = Ctx.makeBasicBlock();
  auto FalseBB = Ctx.makeBasicBlock();
  auto FollowBB = Ctx.makeBasicBlock();

  Ctx.currentBlock()->fallthoughSuccessor() = TrueBB;
  auto Comparison = Rel.genCmp(Ctx);
  auto Branch = Rel.genBranch(Ctx, Comparison, FalseBB);
  Ctx.currentBlock()->terminate(move(Branch));

  Ctx.currentBlock() = TrueBB;
  for (const unique_ptr<Stmt> &S : Then) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->terminate(make_unique<BraInstruction>(Ctx.genInstructionId(), FollowBB));
  }

  Ctx.currentBlock() = FalseBB;
  for (const unique_ptr<Stmt> &S : Else) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->fallthoughSuccessor() = FollowBB;
  }

  Ctx.currentBlock() = FollowBB;
}

WhileStmt::WhileStmt(Relation Rel, vector<unique_ptr<Stmt>> Body) : Rel(move(Rel)), Body(move(Body)) {}

void WhileStmt::genIr(IrGenContext &Ctx) const {
  BasicBlock *HeaderBB = Ctx.makeBasicBlock();
  Ctx.currentBlock()->fallthoughSuccessor() = HeaderBB;
  Ctx.currentBlock() = HeaderBB;

  auto Comparison = Rel.genCmp(Ctx);
  BasicBlock *BodyBB = Ctx.makeBasicBlock();
  Ctx.currentBlock()->fallthoughSuccessor() = BodyBB;
  BasicBlock *FollowBB = Ctx.makeBasicBlock();
  auto HeaderBranch = Rel.genBranch(Ctx, Comparison, FollowBB);
  HeaderBB->terminate(move(HeaderBranch));

  Ctx.currentBlock() = BodyBB;
  for (const unique_ptr<Stmt> &S : Body) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->terminate(make_unique<BraInstruction>(Ctx.genInstructionId(), HeaderBB));
  }

  Ctx.currentBlock() = FollowBB;
}
