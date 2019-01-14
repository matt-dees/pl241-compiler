#include "Stmt.h"
#include "IrGenContext.h"

using namespace cs241c;

namespace {
class ExprLoader : public ExprVisitor {
  IrGenContext *Ctx;

public:
  Value *Result;

  explicit ExprLoader(IrGenContext *Ctx) : Ctx(Ctx) {}

  void visit(ConstantExpr *E) override {
    Value *Constant = E->genIr(*Ctx);
    Value *Zero = Ctx->makeConstant(0);
    Result = Ctx->makeInstruction<AddInstruction>(Constant, Zero);
  }

  void visit(VarDesignator *E) override {
    Value *Variable = E->genIr(*Ctx);
    Value *Zero = Ctx->makeConstant(0);
    Result = Ctx->makeInstruction<AddInstruction>(Variable, Zero);
  }

  void visit(ArrayDesignator *E) override { Result = E->genIr(*Ctx); }

  void visit(FunctionCall *E) override { Result = E->genIr(*Ctx); }

  void visit(MathExpr *E) override { Result = E->genIr(*Ctx); }
};
} // namespace

Assignment::Assignment(std::unique_ptr<Designator> Lhs,
                       std::unique_ptr<Expr> Rhs)
    : Lhs(move(Lhs)), Rhs(move(Rhs)) {}

void Assignment::genIr(IrGenContext &Ctx) const {
  ExprLoader Loader(&Ctx);
  Rhs->visit(&Loader);
  Lhs->genStore(Ctx, Loader.Result);
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> E) : E(move(E)) {}

void ReturnStmt::genIr(IrGenContext &Ctx) const {
  ExprLoader Loader(&Ctx);
  E->visit(&Loader);
  Ctx.makeInstruction<RetInstruction>(Loader.Result);
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
