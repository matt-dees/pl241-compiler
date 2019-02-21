#include "FunctionAnalyzer.h"

using namespace cs241c;
DominatorTree *FunctionAnalyzer::dominatorTree(Function *F) {
  if (DTMap.find(F) == DTMap.end()) {
    return nullptr;
  }
  return DTMap.at(F).get();
}

InterferenceGraph *FunctionAnalyzer::interferenceGraph(Function *F) {
  if (IGMap.find(F) == IGMap.end()) {
    return nullptr;
  }
  return IGMap.at(F).get();
}