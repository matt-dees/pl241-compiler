#include "IntegrityCheckPass.h"
#include "Module.h"
#include <algorithm>
#include <sstream>

using namespace cs241c;
using namespace std;

void IntegrityCheckPass::run(Module &M) {
  for (auto &F : M.functions()) {
    for (auto &BB : F->basicBlocks()) {
      auto &Predecessors = BB->predecessors();
      for (auto Pred : Predecessors) {
        auto FollowersPred = Pred->terminator()->followingBlocks();
        if (count(FollowersPred.begin(), FollowersPred.end(), BB.get()) != 1) {
          stringstream ErrorMessage;
          ErrorMessage << "Block " << BB->name() << " and predecessor " << Pred->name()
                       << " are not connected correctly.";
          throw logic_error(ErrorMessage.str());
        }
      }

      auto Followers = BB->terminator()->followingBlocks();
      for (auto Follower : Followers) {
        auto &PredecessorsFollower = Follower->predecessors();
        if (count(PredecessorsFollower.begin(), PredecessorsFollower.end(), BB.get()) != 1) {
          stringstream ErrorMessage;
          ErrorMessage << "Block " << BB->name() << " and follower " << Follower->name()
                       << " are not connected correctly.";
          throw logic_error(ErrorMessage.str());
        }
      }
    }
  }
}
