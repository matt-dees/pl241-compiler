#include "DominatorTree.h"
#include "IrGenContext.h"
#include "Module.h"
#include <catch.hpp>

using namespace cs241c;
using namespace std;

TEST_CASE("Test Dominator Tree Generation 1") {
  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext Context(CompilationUnit.get());

  auto F = make_unique<Function>("main", vector<unique_ptr<LocalVariable>>{});
  Function *FPtr = F.get();
  Context.declare(move(F));

  // Test with 4 BasicBlocks
  BasicBlock *BB1 = Context.makeBasicBlock();
  BasicBlock *BB2 = Context.makeBasicBlock();
  BasicBlock *BB3 = Context.makeBasicBlock();
  BasicBlock *BB4 = Context.makeBasicBlock();

  Context.currentBlock() = BB1;
  BB1->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB2));

  Context.currentBlock() = BB2;
  BB2->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB3));

  Context.currentBlock() = BB3;
  BB3->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB4));

  Context.currentBlock() = BB4;
  BB4->terminate(make_unique<BasicBlockTerminator>(InstructionType::End, Context.genInstructionId()));

  DominatorTree D;
  D.buildDominatorTree(*FPtr);
  CHECK(true);
}

TEST_CASE("Test Dominator Tree Generation 2") {
  auto CompilationUnit = make_unique<Module>("program");
  IrGenContext Context(CompilationUnit.get());

  auto F = make_unique<Function>("main", vector<unique_ptr<LocalVariable>>{});
  Function *FPtr = F.get();
  Context.declare(move(F));

  // Test with 4 BasicBlocks
  BasicBlock *BB1 = Context.makeBasicBlock();
  BasicBlock *BB2 = Context.makeBasicBlock();
  BasicBlock *BB3 = Context.makeBasicBlock();
  BasicBlock *BB4 = Context.makeBasicBlock();
  BasicBlock *BB5 = Context.makeBasicBlock();

  Context.currentBlock() = BB1;
  BB1->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB2));

  Context.currentBlock() = BB2;
  auto Cmp = make_unique<Instruction>(InstructionType::Cmp, Context.genInstructionId(), Context.makeConstant(5),
                                      Context.makeConstant(5));
  Instruction *CmpP = Cmp.get();
  BB2->appendInstruction(move(Cmp));

  BB2->fallthoughSuccessor() = BB3;
  BB2->terminate(make_unique<ConditionalBlockTerminator>(InstructionType::Bne, Context.genInstructionId(), CmpP, BB4));

  Context.currentBlock() = BB3;

  BB3->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB5));

  Context.currentBlock() = BB4;

  BB4->terminate(make_unique<BraInstruction>(Context.genInstructionId(), BB5));

  Context.currentBlock() = BB5;
  BB5->terminate(make_unique<BasicBlockTerminator>(InstructionType::End, Context.genInstructionId()));
  DominatorTree D;
  D.buildDominatorTree(*FPtr);
  CHECK(true);
}
