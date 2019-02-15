#include "RegisterAllocation.h"
#include <iostream>

using namespace cs241c;
using namespace std;

InterferenceGraph RegisterAllocation::buildInterferenceGraph(Function &F) {
  InterferenceGraph IG;
  Function::bottom_up_iterator BottomUpIter(F);
  for (auto BBIter : BottomUpIter) {
    cout << BBIter->toString() << "\n";
  }
  return IG;
}

void InterferenceGraph::writeGraph(ofstream &) { return; }
