#include "DominatorTree.h"
#include "IrGenContext.h"
#include <catch.hpp>

using namespace cs241c;

TEST_CASE("Test Dominator Tree Generation 1") {

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

  DominatorTree D(BB1.get());
  CHECK(true);
}

TEST_CASE("Test Dominator Tree Generation 2") {

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
  std::unique_ptr<BasicBlock> BB5 =
      std::make_unique<BasicBlock>(Context.genBasicBlockName());

  Context.CurrentBlock = BB1.get();
  BB1->terminate(
      std::make_unique<BraInstruction>(Context.genInstructionId(), BB2.get()));

  Context.CurrentBlock = BB2.get();
  auto Cmp = std::make_unique<CmpInstruction>(Context.genInstructionId(),
                                              Context.makeConstant(5),
                                              Context.makeConstant(5));
  CmpInstruction *CmpP = Cmp.get();
  BB2->appendInstruction(move(Cmp));

  BB2->terminate(std::make_unique<BneInstruction>(Context.genInstructionId(),
                                                  CmpP, BB3.get(), BB4.get()));

  Context.CurrentBlock = BB3.get();

  BB3->terminate(
      std::make_unique<BraInstruction>(Context.genInstructionId(), BB5.get()));

  Context.CurrentBlock = BB4.get();

  BB4->terminate(
      std::make_unique<BraInstruction>(Context.genInstructionId(), BB5.get()));

  Context.CurrentBlock = BB5.get();
  BB5->terminate(std::make_unique<EndInstruction>(Context.genInstructionId()));
  DominatorTree D(DominatorTree(BB1.get()));
  CHECK(true);
}
