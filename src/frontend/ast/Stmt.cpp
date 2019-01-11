#include "Stmt.h"

using namespace cs241c;

Assignment::Assignment(std::unique_ptr<Designator> Lhs,
                       std::unique_ptr<Expr> Rhs)
    : Lhs(move(Lhs)), Rhs(move(Rhs)) {}

namespace {
class AssignmentGenerator : public ExprVisitor {
  IrGenContext *Ctx;
  BasicBlock *BB;

public:
  Value *Result;

  explicit AssignmentGenerator(IrGenContext *Ctx, BasicBlock *BB)
      : Ctx(Ctx), BB(BB) {}

  void visit(ConstantExpr *E) override {
    Value *Constant = E->genIr(*Ctx);
    Value *Zero = Ctx->makeConstant(0);
    Result = Ctx->makeInstruction<AddInstruction>(Constant, Zero, BB);
  }

  void visit(VarDesignator *E) override {
    Value *Variable = E->genIr(*Ctx);
    Value *Zero = Ctx->makeConstant(0);
    Result = Ctx->makeInstruction<AddInstruction>(Variable, Zero, BB);
  }

  void visit(ArrayDesignator *E) override { Result = E->genIr(*Ctx); }

  void visit(FunctionCall *E) override { Result = E->genIr(*Ctx); }

  void visit(MathExpr *E) override { Result = E->genIr(*Ctx); }
};
} // namespace

BasicBlock *Assignment::genIr(IrGenContext &Ctx, BasicBlock *BB) const {
  AssignmentGenerator Gen(&Ctx, BB);
  Rhs->visit(&Gen);
  return BB;
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> E) : E(move(E)) {}

BasicBlock *ReturnStmt::genIr(IrGenContext &Ctx, BasicBlock *BB) const {
  throw std::logic_error("Not implemented.");
}

FunctionCallStmt::FunctionCallStmt(FunctionCall CallExpr)
    : CallExpr(std::move(CallExpr)) {}

BasicBlock *FunctionCallStmt::genIr(IrGenContext &Ctx, BasicBlock *BB) const {
  throw std::logic_error("Not implemented.");
}

IfStmt::IfStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Then,
               std::vector<std::unique_ptr<Stmt>> Else)
    : Rel(std::move(Rel)), Then(move(Then)), Else(move(Else)) {}

BasicBlock *IfStmt::genIr(IrGenContext &Ctx, BasicBlock *BB) const {
  throw std::logic_error("Not implemented.");
}

WhileStmt::WhileStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Body)
    : Rel(std::move(Rel)), Body(std::move(Body)) {}

BasicBlock *WhileStmt::genIr(IrGenContext &Ctx, BasicBlock *BB) const {
  throw std::logic_error("Not implemented.");
}
