#include "Computation.h"

using namespace cs241c;

Computation::Computation(std::vector<std::unique_ptr<cs241c::Decl>> Vars,
                         std::vector<std::unique_ptr<cs241c::Func>> Funcs)
    : Vars(move(Vars)), Funcs(move(Funcs)) {}
