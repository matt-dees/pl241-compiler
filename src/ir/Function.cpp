#include "Function.h"

using namespace cs241c;

Function::Function(const std::string &Name)
    : BasicBlocks(std::vector<BasicBlock>({})), Name(Name) {}
