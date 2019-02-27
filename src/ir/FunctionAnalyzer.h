#ifndef CS241C_IR_FUNCTIONANALYZER_H
#define CS241C_IR_FUNCTIONANALYZER_H

#include "DominatorTree.h"
#include "Function.h"
#include "InterferenceGraph.h"
#include "Module.h"
#include "RegisterAllocator.h"

namespace cs241c {
class FunctionAnalyzer {
private:
  std::unordered_map<Function *, std::unique_ptr<DominatorTree>> DTMap;
  std::unordered_map<Function *, std::unique_ptr<InterferenceGraph>> IGMap;
  std::unordered_map<Function *, std::unique_ptr<RegisterAllocator::Coloring>> RAColoring;

  void buildDominatorTree(Function *F);
  void buildInterferenceGraph(Function *F);

public:
  DominatorTree *dominatorTree(Function *);
  InterferenceGraph *interferenceGraph(Function *);
  RegisterAllocator::Coloring *coloring(Function *);

  bool isValueSpilled(Function *, Value *Val);

  void runDominanceAnalytics(Module *M);
  void runInterferenceAnalytics(Module *M);
  void runRegisterAllocation(Module *M);
};
}; // namespace cs241c
#endif