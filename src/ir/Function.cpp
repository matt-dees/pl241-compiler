#include "Function.h"

using namespace cs241c;

Function::Function(std::string Name,
                   std::vector<std::unique_ptr<ConstantValue>> &&Constants,
                   std::vector<std::unique_ptr<LocalVariable>> &&Locals,
                   std::vector<std::unique_ptr<BasicBlock>> &&BasicBlocks)
    : Name(move(Name)), Constants(move(Constants)), Locals(move(Locals)),
      BasicBlocks(move(BasicBlocks)) {}

const std::string &Function::name() const { return Name; }

std::vector<std::unique_ptr<ConstantValue>> &Function::constants() {
  return Constants;
}

const std::vector<std::unique_ptr<BasicBlock>> &Function::basicBlocks() const {
  return BasicBlocks;
}

std::string Function::toString() const { return Name; }
