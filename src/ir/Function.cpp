#include "Function.h"

using namespace cs241c;

Function::Function(std::string Name,
                   std::vector<std::unique_ptr<BasicBlock>> BasicBlocks)
    : Name(std::move(Name)), BasicBlocks(std::move(BasicBlocks)) {}

std::string Function::toString() const { return Name; }
