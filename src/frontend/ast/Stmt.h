#ifndef CS241C_FRONTEND_AST_STMT_H
#define CS241C_FRONTEND_AST_STMT_H

#include "BasicBlock.h"
#include "Expr.h"

namespace cs241c {
class Stmt {
public:
  virtual BasicBlock *genIr(IrGenContext &Ctx, BasicBlock *BB) const = 0;
};

class Assignment : public Stmt {
  std::unique_ptr<Designator> Lhs;
  std::unique_ptr<Expr> Rhs;

public:
  Assignment(std::unique_ptr<Designator> Lhs, std::unique_ptr<Expr> Rhs);
  BasicBlock *genIr(IrGenContext &Ctx, BasicBlock *BB) const override;
};

class ReturnStmt : public Stmt {
  std::unique_ptr<Expr> E;

public:
  ReturnStmt(std::unique_ptr<Expr> E);
  BasicBlock *genIr(IrGenContext &Ctx, BasicBlock *BB) const override;
};

class FunctionCallStmt : public Stmt {
  FunctionCall CallExpr;

public:
  FunctionCallStmt(FunctionCall CallExpr);
  BasicBlock *genIr(IrGenContext &Ctx, BasicBlock *BB) const override;
};

class IfStmt : public Stmt {
  Relation Rel;
  std::vector<std::unique_ptr<Stmt>> Then;
  std::vector<std::unique_ptr<Stmt>> Else;

public:
  IfStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Then,
         std::vector<std::unique_ptr<Stmt>> Else);
  BasicBlock *genIr(IrGenContext &Ctx, BasicBlock *BB) const override;
};

class WhileStmt : public Stmt {
  Relation Rel;
  std::vector<std::unique_ptr<Stmt>> Body;

public:
  WhileStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Body);
  BasicBlock *genIr(IrGenContext &Ctx, BasicBlock *BB) const override;
};
} // namespace cs241c

#endif
