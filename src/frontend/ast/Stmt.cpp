#include "Stmt.h"

cs241c::Assignment::Assignment(std::unique_ptr<cs241c::Designator> Lhs,
                               std::unique_ptr<cs241c::Expr> Rhs)
    : Lhs(move(Lhs)), Rhs(move(Rhs)) {}

cs241c::ReturnStmt::ReturnStmt(std::unique_ptr<cs241c::Expr> E) : E(move(E)) {}

cs241c::FunctionCallStmt::FunctionCallStmt(cs241c::FunctionCall CallExpr)
    : CallExpr(std::move(CallExpr)) {}

cs241c::IfStmt::IfStmt(cs241c::Relation Rel,
                       std::vector<std::unique_ptr<cs241c::Expr>> Then,
                       std::vector<std::unique_ptr<cs241c::Expr>> Else)
    : Rel(std::move(Rel)), Then(move(Then)), Else(move(Else)) {}

cs241c::WhileStmt::WhileStmt(cs241c::Relation Rel,
                             std::vector<std::unique_ptr<cs241c::Expr>> Body)
    : Rel(std::move(Rel)), Body(std::move(Body)) {}
