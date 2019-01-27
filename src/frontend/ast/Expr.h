#ifndef CS241C_FRONTEND_AST_EXPR_H
#define CS241C_FRONTEND_AST_EXPR_H

#include "Value.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class ArrayDesignator;
class BasicBlock;
class BasicBlockTerminator;
class CmpInstruction;
class ConstantExpr;
class FunctionCall;
class IrGenContext;
class MathExpr;
class VarDesignator;

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
  virtual void visit(ExprVisitor *) = 0;
};

template <typename T> class VisitedExpr : public Expr {
  void visit(ExprVisitor *) override;
};

class ConstantExpr : public VisitedExpr<ConstantExpr> {
  int32_t Val;

public:
  ConstantExpr(int32_t Val);

  Value *genIr(IrGenContext &Ctx) const override;
};

class Designator : public Expr {
public:
  virtual void genStore(IrGenContext &Ctx, Value *V) = 0;
};

template <typename T> class VisitedDesignator : public Designator {
  void visit(ExprVisitor *) override;
};

class VarDesignator : public VisitedDesignator<VarDesignator> {
  std::string Ident;

public:
  VarDesignator(std::string Ident);

  Value *genIr(IrGenContext &Ctx) const override;
  void genStore(IrGenContext &Ctx, Value *V) override;
};

class ArrayDesignator : public VisitedDesignator<ArrayDesignator> {
  std::string Ident;
  std::vector<std::unique_ptr<Expr>> Dim;

  Value* calculateMemoryOffset(IrGenContext &) const;

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

  CmpInstruction *genCmp(IrGenContext &Ctx) const;
  std::unique_ptr<BasicBlockTerminator> genBranch(IrGenContext &Ctx,
                                                  CmpInstruction *Cmp,
                                                  BasicBlock *Then,
                                                  BasicBlock *Else) const;
};
} // namespace cs241c

#endif
