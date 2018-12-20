#ifndef CS241C_FRONTEND_AST_DECL_H
#define CS241C_FRONTEND_AST_DECL_H

#include "Stmt.h"
#include <memory>
#include <string>
#include <vector>

namespace cs241c {
class Decl {};

class IntDecl : public Decl {
  std::string Ident;

public:
  IntDecl(std::string Ident);
};

class ArrayDecl : public Decl {
  std::string Ident;
  std::vector<int32_t> Dim;

public:
  ArrayDecl(std::string Ident, std::vector<int32_t> Dim);
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
};
} // namespace cs241c

#endif
