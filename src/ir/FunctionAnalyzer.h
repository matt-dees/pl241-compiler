#ifndef CS241C_IR_FUNCTIONANALYZER_H
#define CS241C_IR_FUNCTIONANALYZER_H

#include "DominatorTree.h"
#include "Function.h"
#include "InterferenceGraph.h"
#include "RegisterAllocator.h"

namespace cs241c {
class FunctionAnalyzer {
private:
  std::unordered_map<Function *, std::unique_ptr<DominatorTree>> DTMap;
  std::unordered_map<Function *, std::unique_ptr<InterferenceGraph>> IGMap;

public:
  DominatorTree *dominatorTree(Function *);
  InterferenceGraph *interferenceGraph(Function *);
};
}; // namespace cs241c
#endif