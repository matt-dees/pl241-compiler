#include "Expr.h"
#include <functional>
#include <stdexcept>

using namespace cs241c;

ConstantExpr::ConstantExpr(int32_t Val) : Val(Val) {}

Value *ConstantExpr::genIr(IrGenContext &Gen) { return Gen.makeConstant(Val); }

VarDesignator::VarDesignator(std::string Ident) : Ident(move(Ident)) {}

Value *VarDesignator::genIr(IrGenContext &Gen) {
  return Gen.lookupVariable(Ident);
}

ArrayDesignator::ArrayDesignator(std::string Ident,
                                 std::vector<std::unique_ptr<Expr>> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {
  if (this->Dim.empty()) {
    throw std::logic_error("Array with no dimensions");
  }
}

Value *ArrayDesignator::genIr(IrGenContext &) {
  throw std::runtime_error("Array access not implemented.");
}

FunctionCall::FunctionCall(std::string Ident,
                           std::vector<std::unique_ptr<Expr>> Args)
    : Ident(move(Ident)), Args(move(Args)) {}

Value *FunctionCall::genIr(IrGenContext &) {
  throw std::runtime_error("Function calls not implemented.");
}

MathExpr::MathExpr(MathExpr::Operation Op, std::unique_ptr<Expr> Left,
                   std::unique_ptr<Expr> Right)
    : Op(Op), Left(move(Left)), Right(move(Right)) {}

Value *MathExpr::genIr(IrGenContext &Gen) {
  Value *X = Left->genIr(Gen);
  Value *Y = Right->genIr(Gen);
  throw std::runtime_error("Math expressions not implemented.");
}

Relation::Relation(Relation::Type T, std::unique_ptr<Expr> Left,
                   std::unique_ptr<Expr> Right)
    : T(T), Left(move(Left)), Right(move(Right)) {}
