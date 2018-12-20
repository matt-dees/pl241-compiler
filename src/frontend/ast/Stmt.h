#ifndef CS241C_FRONTEND_AST_STMT_H
#define CS241C_FRONTEND_AST_STMT_H

#include "Expr.h"

namespace cs241c {
class Stmt {};

class Assignment : public Stmt {
  std::unique_ptr<Designator> Lhs;
  std::unique_ptr<Expr> Rhs;

public:
  Assignment(std::unique_ptr<Designator> Lhs, std::unique_ptr<Expr> Rhs);
};

class ReturnStmt : public Stmt {
  std::unique_ptr<Expr> E;

public:
  ReturnStmt(std::unique_ptr<Expr> E);
};

class FunctionCallStmt : public Stmt {
  FunctionCall CallExpr;

public:
  FunctionCallStmt(FunctionCall CallExpr);
};

class IfStmt : public Stmt {
  Relation Rel;
  std::vector<std::unique_ptr<Stmt>> Then;
  std::vector<std::unique_ptr<Stmt>> Else;

public:
  IfStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Then,
         std::vector<std::unique_ptr<Stmt>> Else);
};

class WhileStmt : public Stmt {
  Relation Rel;
  std::vector<std::unique_ptr<Stmt>> Body;

public:
  WhileStmt(Relation Rel, std::vector<std::unique_ptr<Stmt>> Body);
};
} // namespace cs241c

#endif
