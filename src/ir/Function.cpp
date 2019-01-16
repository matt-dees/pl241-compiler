#include "Function.h"

using namespace cs241c;

Function::Function(std::string Name,
                   std::vector<std::unique_ptr<BasicBlock>> &&BasicBlocks)
    : Name(move(Name)), BasicBlocks(move(BasicBlocks)) {}

std::string Function::toString() const { return Name; }
