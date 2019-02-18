#include "ConstExprEvalPass.h"
#include "Function.h"
#include "Module.h"
#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace cs241c;
using namespace std;

namespace {
array<InstructionType, 5> ArithmethicInstrs{InstructionType::Neg, InstructionType::Add, InstructionType::Sub,
                                            InstructionType::Mul, InstructionType::Div};

bool canEvaluate(const Instruction *I) {
  auto Arguments = I->arguments();
  bool ConstArgs = all_of(Arguments.begin(), Arguments.end(),
                          [](Value *Arg) { return dynamic_cast<ConstantValue *>(Arg) != nullptr; });
  if (!ConstArgs) {
    return false;
  }

  if (find(ArithmethicInstrs.begin(), ArithmethicInstrs.end(), I->InstrT) != ArithmethicInstrs.end()) {
    if (I->InstrT != InstructionType::Div || dynamic_cast<ConstantValue *>(Arguments[1])->Val != 0) {
      return true;
    }
  }

  if (I->InstrT == InstructionType::Phi) {
    if (dynamic_cast<ConstantValue *>(Arguments[0])->Val == dynamic_cast<ConstantValue *>(Arguments[1])->Val) {
      return true;
    }
  }

  return false;
}

int evaluate(Instruction *I) {
  auto Arguments = I->arguments();

  int Arg0 = dynamic_cast<ConstantValue *>(Arguments[0])->Val;

  if (I->InstrT == InstructionType::Neg) {
    return -Arg0;
  }

  int Arg1 = dynamic_cast<ConstantValue *>(Arguments[1])->Val;
  if (I->InstrT == InstructionType::Add) {
    return Arg0 + Arg1;
  }
  if (I->InstrT == InstructionType::Sub) {
    return Arg0 - Arg1;
  }
  if (I->InstrT == InstructionType::Mul) {
    return Arg0 * Arg1;
  }
  if (I->InstrT == InstructionType::Div) {
    return Arg0 / Arg1;
  }

  if (I->InstrT == InstructionType::Phi) {
    return Arg0;
  }

  throw logic_error("Cannot evaluate provided instruction.");
}

void process(Function &F) {
  auto &Constants = F.constants();

  unordered_map<int, ConstantValue *> ConstantsMap;
  transform(Constants.begin(), Constants.end(), inserter(ConstantsMap, ConstantsMap.end()),
            [](unique_ptr<ConstantValue> &Constant) { return make_pair(Constant->Val, Constant.get()); });

  unordered_map<Value *, Value *> Substitions;

  vector<BasicBlock *> WorkingSet{F.entryBlock()};
  unordered_set<BasicBlock *> MarkedBlocks;

  while (!WorkingSet.empty()) {
    BasicBlock *BB = WorkingSet.back();
    WorkingSet.pop_back();
    MarkedBlocks.insert(BB);

    for (auto &InstructionPtr : BB->instructions()) {
      Instruction *I = InstructionPtr.get();
      I->updateArgs(Substitions);

      if (canEvaluate(I)) {
        int InstructionResult = evaluate(I);

        auto ConstantIt = ConstantsMap.find(InstructionResult);
        if (ConstantIt == ConstantsMap.end()) {
          auto NewConstant = make_unique<ConstantValue>(InstructionResult);
          ConstantsMap[InstructionResult] = NewConstant.get();
          F.constants().push_back(move(NewConstant));
        }

        Substitions[I] = ConstantsMap[InstructionResult];
      }
    }

    for (auto Follower : BB->terminator()->followingBlocks()) {
      auto &Predecessors = Follower->predecessors();
      if (all_of(Predecessors.begin(), Predecessors.end(),
                 [MarkedBlocks](auto &Pred) { return MarkedBlocks.find(Pred) != MarkedBlocks.end(); })) {
        WorkingSet.push_back(Follower);
      }
    }
  }
}
} // namespace

void ConstExprEvalPass::run(Module &M) {
  for (auto &F : M.functions()) {
    process(*F);
  }
}
