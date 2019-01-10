#include "Instruction.h"
#include "IrGenContext.h"
#include "VcgGen.h"
#include <catch.hpp>

using namespace cs241c;

TEST_CASE("Test VCG Graph Generation") {
  BasicBlock BB1 = BasicBlock();
  BB1.Instructions.emplace_back(std::make_unique<NegInstruction>(
      std::make_unique<Value>("X").get(), &BB1));
}