#include "BasicBlock.h"
#include <utility>

using namespace cs241c;

BasicBlock::BasicBlock(std::vector<std::unique_ptr<Instruction>> Instructions)
    : Instructions(std::move(Instructions)), ID(0) {}