#ifndef CS241C_FRONTEND_AST_EXPR_H
#define CS241C_FRONTEND_AST_EXPR_H

#include "Value.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class IrGenContext;
class ConstantExpr;
class VarDesignator;
class ArrayDesignator;
class FunctionCall;
class MathExpr;

class ExprVisitor {
public:
  virtual void visit(ConstantExpr *) = 0;
  virtual void visit(VarDesignator *) = 0;
  virtual void visit(ArrayDesignator *) = 0;
  virtual void visit(FunctionCall *) = 0;
  virtual void visit(MathExpr *) = 0;
};

class Expr {
public:
  virtual ~Expr() = default;
  virtual Value *genIr(IrGenContext &Ctx) const = 0;
  virtual void visit(ExprVisitor *V) = 0;
};

template <typename T> class VisitedExpr : public virtual Expr {
  void visit(ExprVisitor *V) override { V->visit(static_cast<T *>(this)); }
};

class ConstantExpr : public VisitedExpr<ConstantExpr> {
  int32_t Val;

public:
  ConstantExpr(int32_t Val);

  Value *genIr(IrGenContext &Ctx) const override;
};

class Designator : public virtual Expr {
public:
  virtual void genStore(IrGenContext &Ctx, Value *V) = 0;
};

class VarDesignator : public VisitedExpr<VarDesignator>, public Designator {
  std::string Ident;

public:
  VarDesignator(std::string Ident);

  Value *genIr(IrGenContext &Ctx) const override;
  void genStore(IrGenContext &Ctx, Value *V) override;
};

class ArrayDesignator : public VisitedExpr<ArrayDesignator>, public Designator {
  std::string Ident;
  std::vector<std::unique_ptr<Expr>> Dim;

public:
  ArrayDesignator(std::string Ident, std::vector<std::unique_ptr<Expr>> Dim);

  Value *genIr(IrGenContext &Ctx) const override;
  void genStore(IrGenContext &Ctx, Value *V) override;
};

class FunctionCall : public VisitedExpr<FunctionCall> {
  std::string Ident;
  std::vector<std::unique_ptr<Expr>> Args;

public:
  FunctionCall(std::string Ident, std::vector<std::unique_ptr<Expr>> Args);

  Value *genIr(IrGenContext &Ctx) const override;
};

class MathExpr : public VisitedExpr<MathExpr> {
public:
  enum class Operation { Add, Sub, Mul, Div };

private:
  Operation Op;
  std::unique_ptr<Expr> Left;
  std::unique_ptr<Expr> Right;

public:
  MathExpr(Operation Op, std::unique_ptr<Expr> Left,
           std::unique_ptr<Expr> Right);

  Value *genIr(IrGenContext &Ctx) const override;
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

  Value *genCmp(IrGenContext &Ctx) const;
};
} // namespace cs241c

#endif
