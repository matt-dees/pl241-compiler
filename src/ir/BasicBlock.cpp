#include "BasicBlock.h"
#include <utility>

using namespace cs241c;

BasicBlock::BasicBlock(uint32_t ID,
                       std::vector<std::unique_ptr<Instruction>> Instructions)
    : Instructions(std::move(Instructions)), ID(ID) {}