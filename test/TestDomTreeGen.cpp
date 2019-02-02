#include "DominatorTree.h"
#include "IrGenContext.h"
#include "Module.h"
#include <catch.hpp>

using namespace cs241c;
using namespace std;

TEST_CASE("Test Dominator Tree Generation 1") {
  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext Context(CompilationUnit.get());
  // Test with 4 BasicBlocks
  unique_ptr<BasicBlock> BB1 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB2 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB3 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB4 = make_unique<BasicBlock>(Context.genBasicBlockName());

  Context.currentBlock() = BB1.get();
  BB1->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB2.get()));

  Context.currentBlock() = BB2.get();
  BB2->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB3.get()));

  Context.currentBlock() = BB3.get();
  BB3->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB4.get()));

  Context.currentBlock() = BB4.get();
  BB4->terminate(make_unique<EndInstruction>(Context.genInstructionId()));

  DominatorTree D;
  D.buildDominatorTree(BB1.get());
  CHECK(true);
}

TEST_CASE("Test Dominator Tree Generation 2") {
  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext Context(CompilationUnit.get());
  // Test with 4 BasicBlocks
  unique_ptr<BasicBlock> BB1 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB2 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB3 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB4 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB5 = make_unique<BasicBlock>(Context.genBasicBlockName());

  Context.currentBlock() = BB1.get();
  BB1->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB2.get()));

  Context.currentBlock() = BB2.get();
  auto Cmp = make_unique<CmpInstruction>(Context.genInstructionId(), Context.makeConstant(5), Context.makeConstant(5));
  CmpInstruction *CmpP = Cmp.get();
  BB2->appendInstruction(move(Cmp));

  BB2->terminate(make_unique<BneInstruction>(Context.genInstructionId(), CmpP, BB3.get(), BB4.get()));

  Context.currentBlock() = BB3.get();

  BB3->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB5.get()));

  Context.currentBlock() = BB4.get();

  BB4->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB5.get()));

  Context.currentBlock() = BB5.get();
  BB5->terminate(make_unique<EndInstruction>(Context.genInstructionId()));
  DominatorTree D;
  D.buildDominatorTree(BB1.get());
  CHECK(true);
}
