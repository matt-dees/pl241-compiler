#include "Function.h"

using namespace cs241c;

Function::Function(std::string Name,
                   std::vector<std::unique_ptr<BasicBlock>> BasicBlocks)
    : BasicBlocks(std::move(BasicBlocks)), Name(std::move(Name)) {}
