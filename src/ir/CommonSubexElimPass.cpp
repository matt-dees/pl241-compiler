#include "CommonSubexElimPass.h"
#include <iostream>
#include <stack>
using namespace cs241c;
void CommonSubexElimPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(*F);
  }
}

namespace {
struct InstructionHasher {
  std::size_t operator()(Instruction *Instr) const {
    std::size_t hash = 0;
    hash ^= typeid(*Instr).hash_code();
    for (auto &Arg : Instr->arguments()) {
      hash ^= std::hash<size_t>()(reinterpret_cast<size_t>(Arg));
    }
    return hash;
  }
};

struct InstructionEquality {
  bool operator()(Instruction *Instr, Instruction *Other) const {
    return (Instr->arguments() == Other->arguments()) &&
           typeid(Instr) == typeid(Other);
  }
};
} // namespace

void CommonSubexElimPass::run(Function &F) {
  // For each basic block:
  //    For each instruction:
  //      Hash instruction
  //      If Hash already exists:
  //          Add to replacement map if basic block dominates
  //      Else:
  //          Add Hash to map
  //      Replace all args according to replacement map
  //
  std::unordered_map<Instruction *, Instruction *, InstructionHasher,
                     InstructionEquality>
      CandidateInstructions;
  std::unordered_map<Value *, Value *> Replacements;
  std::stack<BasicBlock *> BlocksToExplore;
  std::unordered_set<BasicBlock *> VisitedBlocks;

  BlocksToExplore.push(F.entryBlock());

  while (!BlocksToExplore.empty()) {
    BasicBlock *Runner = BlocksToExplore.top();
    BlocksToExplore.pop();
    for (auto P : Runner->predecessors()) {
      if (VisitedBlocks.find(P) == VisitedBlocks.end()) {
        // Not all Predecessors have been explored
        continue;
      }
    }

    if (VisitedBlocks.find(Runner) != VisitedBlocks.end()) {
      // Already visited
      continue;
    }

    for (auto InstIter = Runner->instructions().begin();
         InstIter != Runner->instructions().end();) {
      if (shouldIgnore(InstIter->get())) {
        InstIter++;
        continue;
      }
      if (CandidateInstructions.find(InstIter->get()) !=
              CandidateInstructions.end() &&
          F.dominatorTree().doesBlockDominate(
              CandidateInstructions.at(InstIter->get())->getOwner(), Runner)) {
        Replacements[InstIter->get()] =
            CandidateInstructions.at(InstIter->get());
        std::cout << "[CSE] Erasing instruction: " << (*InstIter)->toString()
                  << std::endl;
        InstIter = Runner->instructions().erase(InstIter);

      } else {
        CandidateInstructions.erase(InstIter->get());
        CandidateInstructions[InstIter->get()] = InstIter->get();
        InstIter->get()->updateArgs(Replacements);
        InstIter++;
      }
    }
    VisitedBlocks.insert(Runner);
    BasicBlockTerminator *Terminator = Runner->terminator();
    if (Terminator) {
      for (auto &BB : Terminator->followingBlocks()) {
        BlocksToExplore.push(BB);
      }
    }
  }
}

bool CommonSubexElimPass::shouldIgnore(Instruction *I) {
  return dynamic_cast<BasicBlockTerminator *>(I) ||
         dynamic_cast<CallInstruction *>(I);
}