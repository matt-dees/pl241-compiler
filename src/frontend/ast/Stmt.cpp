#include "Stmt.h"
#include "IrGenContext.h"

using namespace cs241c;
using namespace std;

Assignment::Assignment(unique_ptr<Designator> Lhs, unique_ptr<Expr> Rhs) : Lhs(move(Lhs)), Rhs(move(Rhs)) {}

void Assignment::genIr(IrGenContext &Ctx) const {
  auto Storage = Ctx.lookupVariable(Lhs->ident());
  auto Value = Rhs->genIr(Ctx, Storage.Var);
  Lhs->genStore(Ctx, Value);
}

ReturnStmt::ReturnStmt(unique_ptr<Expr> E) : E(move(E)) {}

void ReturnStmt::genIr(IrGenContext &Ctx) const {
  if (Ctx.IsMain) {
    Ctx.currentBlock()->terminate(make_unique<Instruction>(InstructionType::End, Ctx.genInstructionId()));
  } else {
    auto Value = E != nullptr ? E->genIr(Ctx, nullptr) : nullptr;
    Ctx.currentBlock()->terminate(make_unique<Instruction>(InstructionType::Ret, Ctx.genInstructionId(), Value));
  }
}

FunctionCallStmt::FunctionCallStmt(FunctionCall CallExpr) : CallExpr(move(CallExpr)) {}

void FunctionCallStmt::genIr(IrGenContext &Ctx) const { CallExpr.genIr(Ctx, nullptr); }

IfStmt::IfStmt(Relation Rel, vector<unique_ptr<Stmt>> Then, vector<unique_ptr<Stmt>> Else)
    : Rel(move(Rel)), Then(move(Then)), Else(move(Else)) {}

void IfStmt::genIr(IrGenContext &Ctx) const {
  Ctx.currentBlock()->addAttribute(BasicBlockAttr::If);

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
    Ctx.currentBlock()->terminate(make_unique<Instruction>(InstructionType::Bra, Ctx.genInstructionId(), FollowBB));
  }

  Ctx.currentBlock() = FalseBB;
  for (const unique_ptr<Stmt> &S : Else) {
    S->genIr(Ctx);
  }

  if (!Ctx.currentBlock()->isTerminated()) {
    Ctx.currentBlock()->fallthoughSuccessor() = FollowBB;
  }

  Ctx.currentBlock() = FollowBB;
  Ctx.currentBlock()->addAttribute(BasicBlockAttr::Join);
}

WhileStmt::WhileStmt(Relation Rel, vector<unique_ptr<Stmt>> Body) : Rel(move(Rel)), Body(move(Body)) {}

void WhileStmt::genIr(IrGenContext &Ctx) const {
  BasicBlock *HeaderBB = Ctx.makeBasicBlock();
  Ctx.currentBlock()->fallthoughSuccessor() = HeaderBB;
  Ctx.currentBlock() = HeaderBB;

  Ctx.currentBlock()->addAttribute(BasicBlockAttr::While);
  Ctx.currentBlock()->addAttribute(BasicBlockAttr::Join);

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
    Ctx.currentBlock()->terminate(make_unique<Instruction>(InstructionType::Bra, Ctx.genInstructionId(), HeaderBB));
  }

  Ctx.currentBlock() = FollowBB;
}
