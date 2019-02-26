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

Value *ConstantExpr::genIr(IrGenContext &Ctx, Variable *Storage) const { return Ctx.makeConstant(Val); }

Designator::Designator(string Ident) : Ident(move(Ident)) {}

const string &Designator::ident() const { return Ident; }

VarDesignator::VarDesignator(string Ident) : Designator(move(Ident)) {}

Value *VarDesignator::genIr(IrGenContext &Ctx, Variable *Storage) const {
  auto Var = Ctx.lookupVariable(Ident).Var;
  if (Var->isMoveable()) {
    return Var;
  } else {
    auto BaseAddress = Ctx.makeInstruction(T::Adda, Ctx.globalBase(), Var);
    auto Load = Ctx.makeInstruction<MemoryInstruction>(T::Load, Var, BaseAddress);
    Load->storage() = Storage;
    return Load;
  }
}

void VarDesignator::genStore(IrGenContext &Ctx, Value *V) {
  auto Var = Ctx.lookupVariable(Ident).Var;
  if (Var->isMoveable()) {
    Ctx.makeInstruction(InstructionType::Move, V, Var);
  } else {
    auto BaseAddress = Ctx.makeInstruction(T::Adda, Ctx.globalBase(), Var);
    Ctx.makeInstruction<MemoryInstruction>(T::Store, Var, V, BaseAddress);
  }
}

Value *ArrayDesignator::calculateMemoryOffset(IrGenContext &Ctx) const {
  Value *Offset = Dim.front()->genIr(Ctx, nullptr);

  auto Sym = Ctx.lookupVariable(Ident);
  auto DimensionsEnd = Sym.Dimensions.end() - 1;
  for (auto Dimension = Sym.Dimensions.begin(); Dimension != DimensionsEnd; ++Dimension) {
    auto Mul = Ctx.makeInstruction(T::Mul, Offset, Ctx.makeConstant(*Dimension));
    auto DimOffset = Dim.at(Dimension - Sym.Dimensions.begin() + 1)->genIr(Ctx, nullptr);
    auto NewOffset = Ctx.makeInstruction(T::Add, Mul, DimOffset);
    Offset = NewOffset;
  }

  auto ScaledOffset = Ctx.makeInstruction(T::Mul, Offset, Ctx.makeConstant(4));

  return ScaledOffset;
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

Value *ArrayDesignator::genIr(IrGenContext &Ctx, Variable *Storage) const {
  auto Var = Ctx.lookupVariable(Ident).Var;

  Value *BaseAddress = genBaseAddress(Ctx, Var);
  Value *Offset = calculateMemoryOffset(Ctx);
  auto TargetAddress = Ctx.makeInstruction(T::Adda, BaseAddress, Offset);

  auto Load = Ctx.makeInstruction<MemoryInstruction>(T::Load, Var, TargetAddress);
  Load->storage() = Storage;
  return Load;
}

void ArrayDesignator::genStore(IrGenContext &Ctx, Value *V) {
  auto Var = Ctx.lookupVariable(Ident).Var;

  Value *BaseAddress = genBaseAddress(Ctx, Var);
  Value *Offset = calculateMemoryOffset(Ctx);
  auto TargetAddress = Ctx.makeInstruction(T::Adda, BaseAddress, Offset);

  Ctx.makeInstruction<MemoryInstruction>(T::Store, Var, V, TargetAddress);
}

FunctionCall::FunctionCall(string Ident, vector<unique_ptr<Expr>> Args) : Ident(move(Ident)), Args(move(Args)) {}

Value *FunctionCall::genIr(IrGenContext &Ctx, Variable *Storage) const {
  if (Ident == "InputNum") {
    auto InputInstr = Ctx.makeInstruction(InstructionType::Read);
    InputInstr->storage() = Storage;
    return InputInstr;
  }
  if (Ident == "OutputNum") {
    auto ArgVal = Args.front()->genIr(Ctx, nullptr);
    return Ctx.makeInstruction(InstructionType::Write, ArgVal);
  }
  if (Ident == "OutputNewLine") {
    return Ctx.makeInstruction(InstructionType::WriteNL);
  }

  for (const auto &Arg : Args) {
    auto ArgVal = Arg->genIr(Ctx, nullptr);
    Ctx.makeInstruction(InstructionType::Param, ArgVal);
  }

  Function *Target = Ctx.lookupFuncion(Ident);
  auto Call = Ctx.makeInstruction(InstructionType::Call, Target, Ctx.makeConstant(static_cast<int>(Args.size())));
  Call->storage() = Storage;
  return Call;
}

MathExpr::MathExpr(MathExpr::Operation Op, unique_ptr<Expr> Left, unique_ptr<Expr> Right)
    : Op(Op), Left(move(Left)), Right(move(Right)) {}

Value *MathExpr::genIr(IrGenContext &Ctx, Variable *Storage) const {
  Value *X = Left->genIr(Ctx, nullptr);
  Value *Y = Right->genIr(Ctx, nullptr);

  Instruction *Result;
  switch (Op) {
  case Operation::Add:
    Result = Ctx.makeInstruction(T::Add, X, Y);
    break;
  case Operation::Sub:
    Result = Ctx.makeInstruction(T::Sub, X, Y);
    break;
  case Operation::Mul:
    Result = Ctx.makeInstruction(T::Mul, X, Y);
    break;
  case Operation::Div:
    Result = Ctx.makeInstruction(T::Div, X, Y);
    break;
  }
  Result->storage() = Storage;
  return Result;
}

Relation::Relation(Relation::Type T, unique_ptr<Expr> Left, unique_ptr<Expr> Right)
    : T(T), Left(move(Left)), Right(move(Right)) {}

Instruction *Relation::genCmp(IrGenContext &Ctx) const {
  Value *X = Left->genIr(Ctx, nullptr);
  Value *Y = Right->genIr(Ctx, nullptr);
  return Ctx.makeInstruction(T::Cmp, X, Y);
}

namespace {
using RelT = Relation::Type;
unique_ptr<Instruction> makeBranch(IrGenContext &Ctx, RelT T, Instruction *Cmp, BasicBlock *Target) {
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
  auto Terminator = make_unique<Instruction>(InstrT, Id, Cmp, Target);
  return Terminator;
}
} // namespace

unique_ptr<Instruction> Relation::genBranch(IrGenContext &Ctx, Instruction *Cmp, BasicBlock *Target) const {
  return makeBranch(Ctx, T, Cmp, Target);
}
