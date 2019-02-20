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

ConstantExpr::ConstantExpr(int32_t Val) : Val(Val) {}

Value *ConstantExpr::genIr(IrGenContext &Ctx) const { return Ctx.makeConstant(Val); }

Designator::Designator(string Ident) : Ident(move(Ident)) {}

const string &Designator::ident() const { return Ident; }

VarDesignator::VarDesignator(string Ident) : Designator(move(Ident)) {}

Value *VarDesignator::genIr(IrGenContext &Ctx) const {
  auto Var = Ctx.lookupVariable(Ident).Var;
  if (Var->isMoveable()) {
    return Var;
  } else {
    auto BaseAddress = Ctx.makeInstruction(T::Adda, Ctx.globalBase(), Var);
    return Ctx.makeInstruction<MemoryInstruction>(T::Load, Var, BaseAddress);
  }
}

void VarDesignator::genStore(IrGenContext &Ctx, Value *V) {
  auto Var = Ctx.lookupVariable(Ident).Var;
  if (Var->isMoveable()) {
    Ctx.makeInstruction<MoveInstruction>(V, Var);
  } else {
    auto BaseAddress = Ctx.makeInstruction(T::Adda, Ctx.globalBase(), Var);
    Ctx.makeInstruction<MemoryInstruction>(T::Store, Var, V, BaseAddress);
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

ArrayDesignator::ArrayDesignator(string Ident, vector<unique_ptr<Expr>> Dim) : Designator(move(Ident)), Dim(move(Dim)) {
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
  auto TargetAddress = Ctx.makeInstruction(T::Adda, BaseAddress, Offset);
  return Ctx.makeInstruction<MemoryInstruction>(T::Load, Var, TargetAddress);
}

void ArrayDesignator::genStore(IrGenContext &Ctx, Value *V) {
  auto Var = Ctx.lookupVariable(Ident).Var;
  Value *BaseAddress = genBaseAddress(Ctx, Var);
  Value *Offset = calculateMemoryOffset(Ctx);
  auto TargetAddress = Ctx.makeInstruction(T::Adda, BaseAddress, Offset);
  Ctx.makeInstruction<MemoryInstruction>(T::Store, Var, V, TargetAddress);
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

  for (const auto &Arg : Args) {
    Ctx.makeInstruction(InstructionType::Param, Arg->genIr(Ctx));
  }

  Function *Target = Ctx.lookupFuncion(Ident);
  return Ctx.makeInstruction(InstructionType::Call, Target, Ctx.makeConstant(static_cast<int>(Args.size())));
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

Instruction *Relation::genCmp(IrGenContext &Ctx) const {
  Value *X = Left->genIr(Ctx);
  Value *Y = Right->genIr(Ctx);
  return Ctx.makeInstruction(T::Cmp, X, Y);
}

namespace {
using RelT = Relation::Type;
unique_ptr<BasicBlockTerminator> makeBranch(IrGenContext &Ctx, RelT T, Instruction *Cmp, BasicBlock *Target) {
  static const array<pair<RelT, InstructionType>, 6> OpToInstrT{{
      {RelT::Eq, InstructionType::Bne},
      {RelT::Ne, InstructionType::Beq},
      {RelT::Lt, InstructionType::Bge},
      {RelT::Ge, InstructionType::Blt},
      {RelT::Le, InstructionType::Bgt},
      {RelT::Gt, InstructionType::Ble},
  }};

  InstructionType InstrT = find_if(OpToInstrT.begin(), OpToInstrT.end(),
                                   [T](pair<RelT, InstructionType> Mapping) { return Mapping.first == T; })
                               ->second;

  int Id = Ctx.genInstructionId();
  auto Terminator = make_unique<ConditionalBlockTerminator>(InstrT, Id, Cmp, Target);
  return Terminator;
}
} // namespace

unique_ptr<BasicBlockTerminator> Relation::genBranch(IrGenContext &Ctx, Instruction *Cmp, BasicBlock *Target) const {
  return makeBranch(Ctx, T, Cmp, Target);
}
