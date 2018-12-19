#include "Decl.h"

using namespace cs241c;

IntDecl::IntDecl(std::string Ident) : Ident(move(Ident)) {}

ArrayDecl::ArrayDecl(std::string Ident, std::vector<int32_t> Dim)
    : Ident(move(Ident)), Dim(move(Dim)) {}

Func::Func(Func::Type T, std::string Ident, std::vector<std::string> Params,
           std::vector<IntDecl> Vars, std::vector<std::unique_ptr<Stmt>> Stmts)
    : T(T), Ident(move(Ident)), Params(move(Params)), Vars(move(Vars)),
      Stmts(move(Stmts)) {}
