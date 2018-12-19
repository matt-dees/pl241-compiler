#ifndef CS241C_FRONTEND_AST_EXPR_H
#define CS241C_FRONTEND_AST_EXPR_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class Expr {};

class ConstantExpr : public Expr {
  int32_t Val;

public:
  ConstantExpr(int32_t Val);
};

class Designator : public Expr {};

class VarDesignator : public Designator {
  std::string Ident;

public:
  VarDesignator(std::string Ident);
};

class ArrayDesignator : public Designator {
  std::string Ident;
  std::vector<std::unique_ptr<Expr>> Dim;

public:
  ArrayDesignator(std::string Ident, std::vector<std::unique_ptr<Expr>> Dim);
};

class FunctionCall : public Expr {
  std::string Ident;
  std::vector<std::unique_ptr<Expr>> Args;

public:
  FunctionCall(std::string Ident, std::vector<std::unique_ptr<Expr>> Args);
};

class MathExpr : public Expr {
public:
  enum class Operation { Add, Sub, Mul, Div };

private:
  Operation Op;
  std::unique_ptr<Expr> Left;
  std::unique_ptr<Expr> Right;

public:
  MathExpr(Operation Op, std::unique_ptr<Expr> Left,
           std::unique_ptr<Expr> Right);
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
};
} // namespace cs241c

#endif
