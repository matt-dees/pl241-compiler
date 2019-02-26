#ifndef CS241C_FRONTEND_AST_EXPR_H
#define CS241C_FRONTEND_AST_EXPR_H

#include "Value.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class BasicBlock;
class Instruction;
class IrGenContext;
class Variable;

class Expr {
public:
  virtual ~Expr() = default;
  virtual Value *genIr(IrGenContext &Ctx, Variable *Storage) const = 0;
};

class ConstantExpr : public Expr {
  int32_t Val;

public:
  ConstantExpr(int32_t Val);

  Value *genIr(IrGenContext &Ctx, Variable *Storage) const override;
};

class Designator : public Expr {
protected:
  std::string Ident;

  Designator(std::string Ident);

public:
  const std::string &ident() const;
  virtual void genStore(IrGenContext &Ctx, Value *V) = 0;
};

class VarDesignator : public Designator {
public:
  VarDesignator(std::string Ident);

  Value *genIr(IrGenContext &Ctx, Variable *Storage) const override;
  void genStore(IrGenContext &Ctx, Value *V) override;
};

class ArrayDesignator : public Designator {
  std::vector<std::unique_ptr<Expr>> Dim;

  Value *calculateMemoryOffset(IrGenContext &) const;

public:
  ArrayDesignator(std::string Ident, std::vector<std::unique_ptr<Expr>> Dim);

  Value *genIr(IrGenContext &Ctx, Variable *Storage) const override;
  void genStore(IrGenContext &Ctx, Value *V) override;
};

class FunctionCall : public Expr {
  std::string Ident;
  std::vector<std::unique_ptr<Expr>> Args;

public:
  FunctionCall(std::string Ident, std::vector<std::unique_ptr<Expr>> Args);

  Value *genIr(IrGenContext &Ctx, Variable *Storage) const override;
};

class MathExpr : public Expr {
public:
  enum class Operation { Add, Sub, Mul, Div };

private:
  Operation Op;
  std::unique_ptr<Expr> Left;
  std::unique_ptr<Expr> Right;

public:
  MathExpr(Operation Op, std::unique_ptr<Expr> Left, std::unique_ptr<Expr> Right);

  Value *genIr(IrGenContext &Ctx, Variable *Storage) const override;
};

class Relation {
public:
  enum class Type { Eq, Ne, Lt, Le, Ge, Gt };

private:
  Type T;
  std::unique_ptr<Expr> Left;
  std::unique_ptr<Expr> Right;

public:
  Relation(Type T, std::unique_ptr<Expr> Left, std::unique_ptr<Expr> Right);

  Instruction *genCmp(IrGenContext &Ctx) const;
  std::unique_ptr<Instruction> genBranch(IrGenContext &Ctx, Instruction *Cmp, BasicBlock *Target) const;
};
} // namespace cs241c

#endif
