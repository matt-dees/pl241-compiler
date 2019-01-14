#include "Expr.h"
#include "IrGenContext.h"
#include <functional>
#include <stdexcept>

using namespace cs241c;

ConstantExpr::ConstantExpr(int32_t Val) : Val(Val) {}

Value *ConstantExpr::genIr(IrGenContext &Ctx) const {
  return Ctx.makeConstant(Val);
}

VarDesignator::VarDesignator(std::string Ident) : Ident(move(Ident)) {}

Value *VarDesignator::genIr(IrGenContext &Ctx) const {
  return Ctx.lookupVariable(Ident);
}

void VarDesignator::genStore(IrGenContext &Ctx, Value *V) {}

ArrayDesignator::ArrayDesignator(std::string Ident,
                                 std::vector<std::unique_ptr<Expr>> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {
  if (this->Dim.empty()) {
    throw std::logic_error("Array with no dimensions");
  }
}

void ArrayDesignator::genStore(IrGenContext &Ctx, Value *V) {}

Value *ArrayDesignator::genIr(IrGenContext &) const {
  throw std::runtime_error("Array access not implemented.");
}

FunctionCall::FunctionCall(std::string Ident,
                           std::vector<std::unique_ptr<Expr>> Args)
    : Ident(move(Ident)), Args(move(Args)) {}

Value *FunctionCall::genIr(IrGenContext &) const {
  throw std::runtime_error("Function calls not implemented.");
}

MathExpr::MathExpr(MathExpr::Operation Op, std::unique_ptr<Expr> Left,
                   std::unique_ptr<Expr> Right)
    : Op(Op), Left(move(Left)), Right(move(Right)) {}

Value *MathExpr::genIr(IrGenContext &Ctx) const {
  Value *X = Left->genIr(Ctx);
  Value *Y = Right->genIr(Ctx);
  switch (Op) {
  case Operation::Add:
    return Ctx.makeInstruction<AddInstruction>(X, Y);
  case Operation::Sub:
    return Ctx.makeInstruction<SubInstruction>(X, Y);
  case Operation::Mul:
    return Ctx.makeInstruction<MulInstruction>(X, Y);
  case Operation::Div:
    return Ctx.makeInstruction<DivInstruction>(X, Y);
  }
}

Relation::Relation(Relation::Type T, std::unique_ptr<Expr> Left,
                   std::unique_ptr<Expr> Right)
    : T(T), Left(move(Left)), Right(move(Right)) {}

Value *Relation::genCmp(IrGenContext &Ctx) const {
  Value *X = Left->genIr(Ctx);
  Value *Y = Right->genIr(Ctx);
  return Ctx.makeInstruction<CmpInstruction>(X, Y);
}
