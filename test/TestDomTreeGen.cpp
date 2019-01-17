#include "DominatorTree.h"
#include "IrGenContext.h"
#include <catch.hpp>

using namespace cs241c;

TEST_CASE("Test Dominator Tree Generation") {

  IrGenContext Context;
  // Test with 4 BasicBlocks
  std::unique_ptr<BasicBlock> BB1 =
      std::make_unique<BasicBlock>(Context.genBasicBlockName());
  std::unique_ptr<BasicBlock> BB2 =
      std::make_unique<BasicBlock>(Context.genBasicBlockName());
  std::unique_ptr<BasicBlock> BB3 =
      std::make_unique<BasicBlock>(Context.genBasicBlockName());
  std::unique_ptr<BasicBlock> BB4 =
      std::make_unique<BasicBlock>(Context.genBasicBlockName());

  BB1->terminate(
      std::make_unique<BraInstruction>(Context.genInstructionId(), BB2.get()));

  BB2->terminate(
      std::make_unique<BraInstruction>(Context.genInstructionId(), BB3.get()));

  BB3->terminate(
      std::make_unique<BraInstruction>(Context.genInstructionId(), BB4.get()));

  BB4->terminate(std::make_unique<EndInstruction>(Context.genInstructionId()));

  DominatorTree(BB1.get());
  CHECK(true);
}
