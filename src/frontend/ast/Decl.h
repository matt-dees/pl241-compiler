#ifndef CS241C_FRONTEND_AST_DECL_H
#define CS241C_FRONTEND_AST_DECL_H

#include "Function.h"
#include "GlobalVariable.h"
#include "Stmt.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class Decl {
public:
  virtual std::unique_ptr<GlobalVariable> genIr(IrGenContext &Ctx) = 0;
};

class IntDecl : public Decl {
  std::string Ident;

public:
  IntDecl(std::string Ident);

  std::unique_ptr<GlobalVariable> genIr(IrGenContext &Ctx) override;
};

class ArrayDecl : public Decl {
  std::string Ident;
  std::vector<int32_t> Dim;

public:
  ArrayDecl(std::string Ident, std::vector<int32_t> Dim);

  std::unique_ptr<GlobalVariable> genIr(IrGenContext &Ctx) override;
};

class Func {
public:
  enum class Type { Function, Procedure };

private:
  Type T;
  std::string Ident;
  std::vector<std::string> Params;
  std::vector<std::unique_ptr<Decl>> Vars;
  std::vector<std::unique_ptr<Stmt>> Stmts;

public:
  Func(Type T, std::string Ident, std::vector<std::string> Params,
       std::vector<std::unique_ptr<Decl>> Vars,
       std::vector<std::unique_ptr<Stmt>> Stmts);

  std::unique_ptr<Function> genIr(IrGenContext &Ctx);
};
} // namespace cs241c

#endif
