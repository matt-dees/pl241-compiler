#include "Instruction.h"
#include "IrGenContext.h"
#include "VcgGen.h"
#include <catch.hpp>
#include <filesystem>
#include <utility>

using namespace cs241c;

TEST_CASE("Test VCG Graph Generation") {

  const std::string TEST_PATH =
      std::string(CS241C_VCG_TEST_DIR) + "/test_vcg_input.vcg";
  std::filesystem::remove(TEST_PATH);

  IrGenContext Context;
  // Test with 4 BasicBlocks
  std::unique_ptr<BasicBlock> BB1 = std::make_unique<BasicBlock>(1);
  std::unique_ptr<BasicBlock> BB2 = std::make_unique<BasicBlock>(2);
  std::unique_ptr<BasicBlock> BB3 = std::make_unique<BasicBlock>(3);
  std::unique_ptr<BasicBlock> BB4 = std::make_unique<BasicBlock>(4);

  // Basic Block 1
  // neg 1
  // add 1, 3
  // sub 10, 15
  // mul 2, 10
  // div 20, 5
  // cmp 0, 0
  // bra 2
  //
  // Edge to BB2
  BB1->Instructions.emplace_back(
      std::make_unique<NegInstruction>(Context.makeConstant(1), BB1.get()));
  BB1->Instructions.emplace_back(std::make_unique<AddInstruction>(
      Context.makeConstant(1), Context.makeConstant(3), BB1.get()));
  BB1->Instructions.emplace_back(std::make_unique<SubInstruction>(
      Context.makeConstant(10), Context.makeConstant(15), BB1.get()));
  BB1->Instructions.emplace_back(std::make_unique<MulInstruction>(
      Context.makeConstant(2), Context.makeConstant(10), BB1.get()));
  BB1->Instructions.emplace_back(std::make_unique<DivInstruction>(
      Context.makeConstant(20), Context.makeConstant(5), BB1.get()));
  BB1->Instructions.emplace_back(std::make_unique<CmpInstruction>(
      Context.makeConstant(0), Context.makeConstant(0), BB1.get()));
  BB1->Instructions.emplace_back(
      std::make_unique<NegInstruction>(Context.makeConstant(2), BB1.get()));
  BB1->Instructions.emplace_back(
      std::make_unique<BranchInstruction>(Context.makeConstant(1), BB1.get()));

  BB1->NextBlocks.push_back(BB2.get());

  // Basic Block 2
  // adda 1000, 2
  // load 2000
  // store 5, 2000
  // move 600, 700
  // phi 1, 2, 3, 4
  // bne 4, 100
  //
  // Edge to BB3 and BB4
  BB2->Instructions.emplace_back(std::make_unique<AddaInstruction>(
      Context.makeConstant(1000), Context.makeConstant(2), BB2.get()));
  BB2->Instructions.emplace_back(
      std::make_unique<LoadInstruction>(Context.makeConstant(2000), BB2.get()));
  BB2->Instructions.emplace_back(std::make_unique<StoreInstruction>(
      Context.makeConstant(5), Context.makeConstant(2000), BB2.get()));
  BB2->Instructions.emplace_back(std::make_unique<MoveInstruction>(
      Context.makeConstant(600), Context.makeConstant(700), BB2.get()));
  BB2->Instructions.emplace_back(std::make_unique<PhiInstruction>(
      std::vector<Value *>({Context.makeConstant(1), Context.makeConstant(2),
                            Context.makeConstant(3), Context.makeConstant(4)}),
      BB2.get()));
  BB2->Instructions.emplace_back(std::make_unique<BranchNotEqualInstruction>(
      Context.makeConstant(5), Context.makeConstant(100), BB2.get()));

  BB2->NextBlocks.push_back(BB3.get());
  BB2->NextBlocks.push_back(BB4.get());

  // Basic Block 3
  // add 1, 15
  // sub 16, 5
  // bra 4
  //
  // Edge to 4
  BB3->Instructions.emplace_back(std::make_unique<AddInstruction>(
      Context.makeConstant(1), Context.makeConstant(15), BB3.get()));
  BB3->Instructions.emplace_back(std::make_unique<SubInstruction>(
      Context.makeConstant(16), Context.makeConstant(5), BB3.get()));
  BB3->Instructions.emplace_back(
      std::make_unique<BranchInstruction>(Context.makeConstant(4), BB3.get()));

  BB3->NextBlocks.push_back(BB4.get());

  // Basic Block 4
  // div 10, 8
  // end
  BB4->Instructions.emplace_back(std::make_unique<DivInstruction>(
      Context.makeConstant(10), Context.makeConstant(8), BB4.get()));
  BB4->Instructions.emplace_back(std::make_unique<EndInstruction>(BB4.get()));

  // Function 1
  //  Basic Block(s) 1
  std::vector<std::unique_ptr<BasicBlock>> F1Blocks;
  F1Blocks.emplace_back(std::move(BB1));
  std::unique_ptr<Function> F1 =
      std::make_unique<Function>("F1", std::move(F1Blocks));

  // Function 2
  //  Basic Block(s) 2-4
  std::vector<std::unique_ptr<BasicBlock>> F2Blocks;
  F2Blocks.emplace_back(std::move(BB2));
  F2Blocks.emplace_back(std::move(BB3));
  F2Blocks.emplace_back(std::move(BB4));
  std::unique_ptr<Function> F2 =
      std::make_unique<Function>("F2", std::move(F2Blocks));

  // Module
  //  Functions 1 and 2

  std::vector<std::unique_ptr<Function>> TestModuleFunctions;
  TestModuleFunctions.emplace_back(std::move(F1));
  TestModuleFunctions.emplace_back(std::move(F2));

  std::vector<std::unique_ptr<GlobalVariable>> TestModuleGlobals;

  std::unique_ptr<Module> M =
      std::make_unique<Module>("TestModule", std::move(TestModuleGlobals),
                               std::move(TestModuleFunctions));
  VcgGen VcgGenerator = VcgGen(M.get());
  VcgGenerator.generate(TEST_PATH);
  CHECK(true);
}
