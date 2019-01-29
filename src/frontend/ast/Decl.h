#ifndef CS241C_FRONTEND_AST_DECL_H
#define CS241C_FRONTEND_AST_DECL_H

#include "Stmt.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class Function;
class GlobalVariable;
class IrGenContext;
class LocalVariable;

class Decl {
public:
  virtual ~Decl() = default;

  virtual void declareGlobal(IrGenContext &Ctx) = 0;
  virtual std::unique_ptr<LocalVariable> declareLocal(IrGenContext &Ctx) = 0;
};

class IntDecl : public Decl {
  std::string Ident;

public:
  IntDecl(std::string Ident);

  void declareGlobal(IrGenContext &Ctx) override;
  std::unique_ptr<LocalVariable> declareLocal(IrGenContext &Ctx) override;
};

class ArrayDecl : public Decl {
  std::string Ident;
  std::vector<int32_t> Dim;

public:
  ArrayDecl(std::string Ident, std::vector<int32_t> Dim);

  void declareGlobal(IrGenContext &Ctx) override;
  std::unique_ptr<LocalVariable> declareLocal(IrGenContext &Ctx) override;
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

  void genIr(IrGenContext &Ctx);
};
} // namespace cs241c

#endif
