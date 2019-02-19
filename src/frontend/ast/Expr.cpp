#include "Expr.h"
#include "Function.h"
#include "Instruction.h"
#include "IrGenContext.h"
#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>
#include <utility>

using namespace cs241c;
using namespace std;

using T = InstructionType;

template <typename T> void VisitedExpr<T>::visit(ExprVisitor *V) { V->visit(static_cast<T *>(this)); }

ConstantExpr::ConstantExpr(int32_t Val) : Val(Val) {}

Value *ConstantExpr::genIr(IrGenContext &Ctx) const { return Ctx.makeConstant(Val); }

template <typename T> void VisitedDesignator<T>::visit(ExprVisitor *V) { V->visit(static_cast<T *>(this)); }

VarDesignator::VarDesignator(string Ident) : Ident(move(Ident)) {}

Value *VarDesignator::genIr(IrGenContext &Ctx) const {
  auto Var = Ctx.lookupVariable(Ident).Var;
  if (Var->isMoveable()) {
    return Var;
  } else {
    auto BaseAddress = Ctx.makeInstruction<AddaInstruction>(Ctx.globalBase(), Var);
    return Ctx.makeInstruction<LoadInstruction>(Var, BaseAddress);
  }
}

void VarDesignator::genStore(IrGenContext &Ctx, Value *V) {
  auto Var = Ctx.lookupVariable(Ident).Var;
  if (Var->isMoveable()) {
    Ctx.makeInstruction<MoveInstruction>(V, Var);
  } else {
    auto BaseAddress = Ctx.makeInstruction<AddaInstruction>(Ctx.globalBase(), Var);
    Ctx.makeInstruction<StoreInstruction>(Var, V, BaseAddress);
  }
}

Value *ArrayDesignator::calculateMemoryOffset(IrGenContext &Ctx) const {
  Value *Offset = Dim.front()->genIr(Ctx);

  auto Sym = Ctx.lookupVariable(Ident);
  auto DimensionsEnd = Sym.Dimensions.end() - 1;
  for (auto Dimension = Sym.Dimensions.begin(); Dimension != DimensionsEnd; ++Dimension) {
    Offset = Ctx.makeInstruction(T::Mul, Offset, Ctx.makeConstant(*Dimension));
    Offset = Ctx.makeInstruction(T::Add, Offset, Dim.at(Dimension - Sym.Dimensions.begin() + 1)->genIr(Ctx));
  }

  Offset = Ctx.makeInstruction(T::Mul, Offset, Ctx.makeConstant(4));

  return Offset;
}

ArrayDesignator::ArrayDesignator(string Ident, vector<unique_ptr<Expr>> Dim) : Ident(move(Ident)), Dim(move(Dim)) {
  if (this->Dim.empty()) {
    throw logic_error("Array with no dimensions");
  }
}

namespace {
Value *genBaseAddress(IrGenContext &Ctx, Variable *Var) {
  if (dynamic_cast<GlobalVariable *>(Var)) {
    return Ctx.makeInstruction(T::Add, Ctx.globalBase(), Var);
  }
  return Var;
}
} // namespace

Value *ArrayDesignator::genIr(IrGenContext &Ctx) const {
  auto Var = Ctx.lookupVariable(Ident).Var;
  Value *BaseAddress = genBaseAddress(Ctx, Var);
  Value *Offset = calculateMemoryOffset(Ctx);
  auto TargetAddress = Ctx.makeInstruction<AddaInstruction>(BaseAddress, Offset);
  return Ctx.makeInstruction<LoadInstruction>(Var, TargetAddress);
}

void ArrayDesignator::genStore(IrGenContext &Ctx, Value *V) {
  auto Var = Ctx.lookupVariable(Ident).Var;
  Value *BaseAddress = genBaseAddress(Ctx, Var);
  Value *Offset = calculateMemoryOffset(Ctx);
  auto TargetAddress = Ctx.makeInstruction<AddaInstruction>(BaseAddress, Offset);
  Ctx.makeInstruction<StoreInstruction>(Var, V, TargetAddress);
}

FunctionCall::FunctionCall(string Ident, vector<unique_ptr<Expr>> Args) : Ident(move(Ident)), Args(move(Args)) {}

Value *FunctionCall::genIr(IrGenContext &Ctx) const {
  if (Ident == "InputNum") {
    return Ctx.makeInstruction(InstructionType::Read);
  }
  if (Ident == "OutputNum") {
    return Ctx.makeInstruction(InstructionType::Write, Args.front()->genIr(Ctx));
  }
  if (Ident == "OutputNewLine") {
    return Ctx.makeInstruction(InstructionType::WriteNL);
  }

  Function *Target = Ctx.lookupFuncion(Ident);

  vector<Value *> Arguments;
  Arguments.push_back(Target);
  transform(Args.begin(), Args.end(), back_inserter(Arguments),
            [&Ctx](const unique_ptr<Expr> &Arg) { return Arg->genIr(Ctx); });

  return Ctx.makeInstruction(InstructionType::Call, move(Arguments));
}

MathExpr::MathExpr(MathExpr::Operation Op, unique_ptr<Expr> Left, unique_ptr<Expr> Right)
    : Op(Op), Left(move(Left)), Right(move(Right)) {}

Value *MathExpr::genIr(IrGenContext &Ctx) const {
  Value *X = Left->genIr(Ctx);
  Value *Y = Right->genIr(Ctx);
  switch (Op) {
  case Operation::Add:
    return Ctx.makeInstruction(T::Add, X, Y);
  case Operation::Sub:
    return Ctx.makeInstruction(T::Sub, X, Y);
  case Operation::Mul:
    return Ctx.makeInstruction(T::Mul, X, Y);
  case Operation::Div:
    return Ctx.makeInstruction(T::Div, X, Y);
  }
  throw logic_error("Invalid value for Op.");
}

Relation::Relation(Relation::Type T, unique_ptr<Expr> Left, unique_ptr<Expr> Right)
    : T(T), Left(move(Left)), Right(move(Right)) {}

CmpInstruction *Relation::genCmp(IrGenContext &Ctx) const {
  Value *X = Left->genIr(Ctx);
  Value *Y = Right->genIr(Ctx);
  return Ctx.makeInstruction<CmpInstruction>(X, Y);
}

namespace {
using RelT = Relation::Type;
static unique_ptr<BasicBlockTerminator> makeBranch(IrGenContext &Ctx, RelT T, CmpInstruction *Cmp, BasicBlock *Then,
                                                   BasicBlock *Else) {
  static const array<pair<RelT, InstructionType>, 6> OpToInstrT{{
      {RelT::Eq, InstructionType::Beq},
      {RelT::Ne, InstructionType::Bne},
      {RelT::Lt, InstructionType::Blt},
      {RelT::Le, InstructionType::Ble},
      {RelT::Ge, InstructionType::Bge},
      {RelT::Gt, InstructionType::Bgt},
  }};

  InstructionType InstrT = find_if(OpToInstrT.begin(), OpToInstrT.end(),
                                   [T](pair<RelT, InstructionType> Mapping) { return Mapping.first == T; })
                               ->second;

  int Id = Ctx.genInstructionId();
  auto Terminator = make_unique<ConditionalBlockTerminator>(InstrT, Id, Cmp, Then, Else);
  return Terminator;
}
} // namespace

unique_ptr<BasicBlockTerminator> Relation::genBranch(IrGenContext &Ctx, CmpInstruction *Cmp, BasicBlock *Then,
                                                     BasicBlock *Else) const {
  return makeBranch(Ctx, T, Cmp, Then, Else);
}
