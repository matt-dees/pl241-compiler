#include "CommonSubexElimPass.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <stack>
#include <type_traits>

using namespace cs241c;
using namespace std;

void CommonSubexElimPass::run(Module &M) {
  for (auto &F : M.functions()) {
    run(*F);
  }
}

namespace {
struct InstructionHasher {
  size_t operator()(const Instruction *Instr) const {
    // FNV-1a
    const uint64_t FNVPrime = 1099511628211;
    uint64_t Hash = 14695981039346656037ull;

    Hash ^= static_cast<underlying_type<InstructionType>::type>(Instr->InstrT);
    Hash *= FNVPrime;

    for (Value *Arg : Instr->arguments()) {
      Hash ^= reinterpret_cast<uintptr_t>(Arg);
      Hash *= FNVPrime;
    }

    return Hash;
  }
};

struct InstructionEquality {
  bool operator()(Instruction *Instr, Instruction *Other) const {
    return (Instr->arguments() == Other->arguments()) &&
           typeid(Instr) == typeid(Other);
  }
};
} // namespace

namespace {
bool shouldIgnore(Instruction *I) {
  static const array<InstructionType, 7> IgnoredInstructions{
      InstructionType::Load,   InstructionType::Store, InstructionType::Param,
      InstructionType::Call,   InstructionType::Read,  InstructionType::Write,
      InstructionType::WriteNL};
  return dynamic_cast<BasicBlockTerminator *>(I) != nullptr ||
         find(IgnoredInstructions.begin(), IgnoredInstructions.end(),
              I->InstrT) != IgnoredInstructions.end();
}

bool shouldExplore(BasicBlock *From, BasicBlock *To,
                   const unordered_set<BasicBlock *> &Visited,
                   DominatorTree *DT) {
  for (auto &P : To->predecessors()) {
    // If the basic block we want to explore has a predecessor that is
    // unexplored and is NOT dominated by us, don't explore it yet.
    if (Visited.find(P) == Visited.end() && !DT->doesBlockDominate(From, To)) {
      return false;
    }
  }
  return true;
}
}; // namespace

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
  unordered_map<Instruction *, Instruction *, InstructionHasher,
                InstructionEquality>
      CandidateInstructions;
  map<ValueRef, ValueRef> Replacements;
  stack<BasicBlock *> BlocksToExplore;
  unordered_set<BasicBlock *> VisitedBlocks;

  BlocksToExplore.push(F.entryBlock());

  while (!BlocksToExplore.empty()) {
    BasicBlock *Runner = BlocksToExplore.top();
    BlocksToExplore.pop();

    if (VisitedBlocks.find(Runner) != VisitedBlocks.end()) {
      // Already visited
      continue;
    }

    for (auto InstIter = Runner->instructions().begin();
         InstIter != Runner->instructions().end();) {
      if (shouldIgnore(InstIter->get())) {
        // If instruction should not be considered for CSE,
        // simply update arguments according to Replacements map and continue.
        InstIter->get()->updateArgs(Replacements);
        InstIter++;
        continue;
      }
      bool HasMatch = CandidateInstructions.find(InstIter->get()) !=
                      CandidateInstructions.end();
      bool MatchIsMe = HasMatch && CandidateInstructions.at(InstIter->get()) ==
                                       InstIter->get();
      if (MatchIsMe) {
        // If this existing entry in the map is this instruction, delete it
        // because our arguments may get updated. The instruction will
        // get re-added to the map later.
        CandidateInstructions.erase(InstIter->get());
      }
      // Update arguments according to Replacements map
      InstIter->get()->updateArgs(Replacements);

      // Rehash instruction because arguments may have changed
      HasMatch = CandidateInstructions.find(InstIter->get()) !=
                 CandidateInstructions.end();
      bool DominatedByMatch =
          HasMatch &&
          FA.dominatorTree(&F)->doesBlockDominate(
              CandidateInstructions.at(InstIter->get())->owner(), Runner);

      if (DominatedByMatch) {
        // If this instruction is dominated by its match, update the
        // Replacements map which will dictate which instructions
        // should be replaced by what values.
        Replacements[InstIter->get()] =
            CandidateInstructions.at(InstIter->get());
        if (PrintDebug)
          cout << "[CSE] " << (*InstIter)->toString() << " --> "
               << CandidateInstructions.at(InstIter->get())->toString() << "\n";
        for (auto &DFEntry : FA.dominatorTree(&F)->dominanceFrontier(Runner)) {
          // Need to revisit all blocks in the dominance frontier.
          VisitedBlocks.erase(DFEntry);
          BlocksToExplore.push(DFEntry);
        }
        // Perform CSE. Delete duplicate instruction.
        InstIter = Runner->instructions().erase(InstIter);
      } else {
        // Update entry in the hash map
        CandidateInstructions[InstIter->get()] = InstIter->get();
        InstIter++;
      }
    }
    VisitedBlocks.insert(Runner);
    for (auto &BB : Runner->successors()) {
      if (shouldExplore(Runner, BB, VisitedBlocks, FA.dominatorTree(&F))) {
        BlocksToExplore.push(BB);
      }
    }
  }
}
