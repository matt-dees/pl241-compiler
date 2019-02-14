#include "IntegrityCheckPass.h"
#include "Module.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <unordered_set>

using namespace cs241c;
using namespace std;

void IntegrityCheckPass::run(Module &M) {
  unordered_set<Value *> Values;
  Values.insert(M.globalBase());
  transform(M.globals().begin(), M.globals().end(), inserter(Values, Values.end()),
            [](auto &UniquePtr) { return UniquePtr.get(); });
  transform(M.functions().begin(), M.functions().end(), inserter(Values, Values.end()),
            [](auto &UniquePtr) { return UniquePtr.get(); });

  for (auto &F : M.functions()) {
    transform(F->constants().begin(), F->constants().end(), inserter(Values, Values.end()),
              [](auto &UniquePtr) { return UniquePtr.get(); });
    transform(F->locals().begin(), F->locals().end(), inserter(Values, Values.end()),
              [](auto &UniquePtr) { return UniquePtr.get(); });
    transform(F->basicBlocks().begin(), F->basicBlocks().end(), inserter(Values, Values.end()),
              [](auto &UniquePtr) { return UniquePtr.get(); });

    for (auto &BB : F->basicBlocks()) {
      transform(BB->instructions().begin(), BB->instructions().end(), inserter(Values, Values.end()),
                [](auto &UniquePtr) { return UniquePtr.get(); });

      auto &Predecessors = BB->predecessors();
      if (Predecessors.size() > 2) {
        stringstream ErrorMessage;
        ErrorMessage << "Block " << BB->name() << " has " << Predecessors.size() << " predecessors.";
        throw logic_error(ErrorMessage.str());
      }

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
        vector<BasicBlock *>::difference_type EdgesToFollower =
            count(PredecessorsFollower.begin(), PredecessorsFollower.end(), BB.get());
        if (EdgesToFollower != 1) {
          stringstream ErrorMessage;
          ErrorMessage << "Block " << BB->name() << " has " << EdgesToFollower << " edges to follower "
                       << Follower->name() << ".";
          throw logic_error(ErrorMessage.str());
        }
      }

      if (BB->terminator() == nullptr) {
        stringstream ErrorMessage;
        ErrorMessage << "Block " << BB->name() << " is not terminated.";
        throw logic_error(ErrorMessage.str());
      }
    }

    for (auto &BB : F->basicBlocks()) {
      for (auto &Instr : BB->instructions()) {
        int ArgIndex = 0;
        for (auto Arg : Instr->arguments()) {
          if (Values.find(Arg) == Values.end()) {
            stringstream ErrorMessage;
            ErrorMessage << "Instruction [" << Instr->name() << ": " << Instr->mnemonic() << "] argument " << ArgIndex
                         << " in block " << BB->name() << " is deleted.";
            throw logic_error(ErrorMessage.str());
          }
          ++ArgIndex;
        }
      }
    }
  }
}
