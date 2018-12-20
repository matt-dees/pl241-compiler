#ifndef CS241C_FRONTEND_AST_COMPUTATION_H
#define CS241C_FRONTEND_AST_COMPUTATION_H

#include "Decl.h"
#include <memory>
#include <vector>

namespace cs241c {
class Computation {
  std::vector<std::unique_ptr<Decl>> Vars;
  std::vector<Func> Funcs;

public:
  Computation(std::vector<std::unique_ptr<Decl>> Vars, std::vector<Func> Funcs);
};
} // namespace cs241c

#endif
