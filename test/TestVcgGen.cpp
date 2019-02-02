#include "Filesystem.h"
#include "Instruction.h"
#include "IrGenContext.h"
#include "VcgGen.h"
#include <catch.hpp>
#include <utility>

using namespace cs241c;
using namespace std;

TEST_CASE("Test VCG Graph Generation") {

  const string TEST_PATH = string(CS241C_VCG_TEST_DIR) + "/test_vcg_input.vcg";
  removeFile(TEST_PATH);

  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext Context(CompilationUnit.get());
  // Test with 4 BasicBlocks
  unique_ptr<BasicBlock> BB1 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB2 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB3 = make_unique<BasicBlock>(Context.genBasicBlockName());
  unique_ptr<BasicBlock> BB4 = make_unique<BasicBlock>(Context.genBasicBlockName());

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
  BB1->appendInstruction(make_unique<NegInstruction>(Context.genInstructionId(), Context.makeConstant(1)));
  BB1->appendInstruction(
      make_unique<AddInstruction>(Context.genInstructionId(), Context.makeConstant(1), Context.makeConstant(3)));
  BB1->appendInstruction(
      make_unique<SubInstruction>(Context.genInstructionId(), Context.makeConstant(10), Context.makeConstant(15)));
  BB1->appendInstruction(
      make_unique<MulInstruction>(Context.genInstructionId(), Context.makeConstant(2), Context.makeConstant(10)));
  BB1->appendInstruction(
      make_unique<DivInstruction>(Context.genInstructionId(), Context.makeConstant(20), Context.makeConstant(5)));
  BB1->appendInstruction(
      make_unique<CmpInstruction>(Context.genInstructionId(), Context.makeConstant(0), Context.makeConstant(0)));
  BB1->appendInstruction(make_unique<NegInstruction>(Context.genInstructionId(), Context.makeConstant(2)));
  BB1->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB2.get()));

  // Basic Block 2
  // adda 1000, 2
  // load 2000
  // store 5, 2000
  // move 600, 700
  // %1 = cmp 5, 5
  // bne %1, 3, 4
  //
  // Edge to BB3 and BB4
  auto Adda =
      make_unique<AddaInstruction>(Context.genInstructionId(), Context.makeConstant(1000), Context.makeConstant(2));
  auto AddaPtr = Adda.get();
  BB2->appendInstruction(move(Adda));
  BB2->appendInstruction(make_unique<LoadInstruction>(Context.genInstructionId(), nullptr, AddaPtr));
  BB2->appendInstruction(
      make_unique<StoreInstruction>(Context.genInstructionId(), nullptr, Context.makeConstant(5), AddaPtr));
  BB2->appendInstruction(
      make_unique<MoveInstruction>(Context.genInstructionId(), Context.makeConstant(600), Context.makeConstant(700)));
  auto Cmp = make_unique<CmpInstruction>(Context.genInstructionId(), Context.makeConstant(5), Context.makeConstant(5));
  CmpInstruction *CmpP = Cmp.get();
  BB2->appendInstruction(move(Cmp));
  BB2->terminate(make_unique<BneInstruction>(Context.genInstructionId(), CmpP, BB3.get(), BB4.get()));

  // Basic Block 3
  // add 1, 15
  // sub 16, 5
  // bra 4
  //
  // Edge to 4
  BB3->appendInstruction(
      make_unique<AddInstruction>(Context.genInstructionId(), Context.makeConstant(1), Context.makeConstant(15)));
  BB3->appendInstruction(
      make_unique<SubInstruction>(Context.genInstructionId(), Context.makeConstant(16), Context.makeConstant(5)));
  BB3->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB4.get()));

  // Basic Block 4
  // div 10, 8
  // end
  BB4->appendInstruction(
      make_unique<DivInstruction>(Context.genInstructionId(), Context.makeConstant(10), Context.makeConstant(8)));
  BB4->terminate(make_unique<EndInstruction>(Context.genInstructionId()));

  // Function 1
  //  Basic Block(s) 1
  vector<unique_ptr<BasicBlock>> F1Blocks;
  F1Blocks.emplace_back(move(BB1));
  unique_ptr<Function> F1 = make_unique<Function>("F1", vector<unique_ptr<ConstantValue>>{},
                                                  vector<unique_ptr<LocalVariable>>{}, move(F1Blocks));

  // Function 2
  //  Basic Block(s) 2-4
  vector<unique_ptr<BasicBlock>> F2Blocks;
  F2Blocks.emplace_back(move(BB2));
  F2Blocks.emplace_back(move(BB3));
  F2Blocks.emplace_back(move(BB4));
  unique_ptr<Function> F2 = make_unique<Function>("F2", vector<unique_ptr<ConstantValue>>{},
                                                  vector<unique_ptr<LocalVariable>>{}, move(F2Blocks));

  // Module
  //  Functions 1 and 2

  vector<unique_ptr<Function>> TestModuleFunctions;
  TestModuleFunctions.emplace_back(move(F1));
  TestModuleFunctions.emplace_back(move(F2));

  vector<unique_ptr<GlobalVariable>> TestModuleGlobals;

  VcgGen VcgGenerator = VcgGen(CompilationUnit.get());
  VcgGenerator.generate(TEST_PATH);
  CHECK(true);
}
