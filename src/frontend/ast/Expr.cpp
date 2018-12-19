#include "Expr.h"
#include <stdexcept>

using namespace cs241c;

ConstantExpr::ConstantExpr(int32_t Val) : Val(Val) {}

VarDesignator::VarDesignator(std::string Ident) : Ident(move(Ident)) {}

ArrayDesignator::ArrayDesignator(std::string Ident,
                                 std::vector<std::unique_ptr<Expr>> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {
  if (this->Dim.empty()) {
    throw std::logic_error("Array with no dimensions");
  }
}

FunctionCall::FunctionCall(std::string Ident,
                           std::vector<std::unique_ptr<Expr>> Args)
    : Ident(move(Ident)), Args(move(Args)) {}

MathExpr::MathExpr(MathExpr::Operation Op, std::unique_ptr<Expr> Left,
                   std::unique_ptr<Expr> Right)
    : Op(Op), Left(move(Left)), Right(move(Right)) {}

Relation::Relation(Relation::Type T, std::unique_ptr<Expr> Left,
                   std::unique_ptr<Expr> Right)
    : T(T), Left(move(Left)), Right(move(Right)) {}
