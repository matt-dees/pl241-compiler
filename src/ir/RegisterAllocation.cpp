#include "RegisterAllocation.h"
#include <iostream>

using namespace cs241c;

InterferenceGraph RegisterAllocation::buildInterferenceGraph(Function &F) {
  InterferenceGraph IG;
  Function::bottom_up_iterator BottomUpIter(F);
  for (auto BBIter : BottomUpIter) {
    std::cout << BBIter->toString() << std::endl;
  }
  return IG;
}

void InterferenceGraph::writeGraph(std::ofstream &OutFileStream) { return; }