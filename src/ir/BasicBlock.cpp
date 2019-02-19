#include "BasicBlock.h"
#include "SSAContext.h"
#include "Variable.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

using namespace cs241c;
using namespace std;

BasicBlock::FallthroughSuccessorProxy::FallthroughSuccessorProxy(BasicBlock *BB) : BB(BB) {}

BasicBlock::FallthroughSuccessorProxy::operator BasicBlock *() const { return BB->FallthroughSuccessor; }

BasicBlock::FallthroughSuccessorProxy &BasicBlock::FallthroughSuccessorProxy::operator=(BasicBlock *Other) {
  BB->updateSuccessor(BB->FallthroughSuccessor, Other);
  return *this;
}

BasicBlock::BasicBlock(string Name, deque<unique_ptr<Instruction>> Instructions)
    : Value(ValueType::BasicBlock), Name(move(Name)), Predecessors({}), Instructions(move(Instructions)) {}

const vector<BasicBlock *> &BasicBlock::predecessors() const { return Predecessors; }

BasicBlock::FallthroughSuccessorProxy BasicBlock::fallthoughSuccessor() { return {this}; }

vector<BasicBlock *> BasicBlock::successors() const {
  vector<BasicBlock *> Result;
  if (FallthroughSuccessor != nullptr) {
    Result.push_back(FallthroughSuccessor);
  }

  auto Terminator = terminator();
  if (Terminator != nullptr) {
    auto Target = Terminator->target();
    if (Target != nullptr) {
      Result.push_back(Target);
    }
  }

  return Result;
}

void BasicBlock::updateSuccessor(BasicBlock *From, BasicBlock *To) {
  if (From != nullptr) {
    auto &FromPreds = From->Predecessors;
    FromPreds.erase(remove(FromPreds.begin(), FromPreds.end(), this), FromPreds.end());
  }

  if (From == FallthroughSuccessor) {
    FallthroughSuccessor = To;
  } else {
    assert(To != nullptr);

    auto Terminator = terminator();
    assert(Terminator != nullptr);

    int TargetPos = Terminator->InstrT == InstructionType::Bra ? 0 : 1;
    Terminator->updateArg(TargetPos, To);
  }

  if (To != nullptr) {
    To->Predecessors.push_back(this);
  }
}

deque<unique_ptr<Instruction>> &BasicBlock::instructions() { return Instructions; }

BasicBlockTerminator *BasicBlock::terminator() const {
  if (Instructions.empty())
    return nullptr;
  return dynamic_cast<BasicBlockTerminator *>(Instructions.back().get());
}

void BasicBlock::appendInstruction(unique_ptr<Instruction> I) {
  I->Owner = this;
  Instructions.push_back(move(I));
}

void BasicBlock::appendPredecessor(BasicBlock *BB) { Predecessors.push_back(BB); }
bool BasicBlock::isTerminated() {
  return !Instructions.empty() && dynamic_cast<BasicBlockTerminator *>(Instructions.back().get());
}

void BasicBlock::terminate(unique_ptr<BasicBlockTerminator> T) {
  static const array<InstructionType, 6> ConditionalBranches{InstructionType::Bne, InstructionType::Beq,
                                                             InstructionType::Ble, InstructionType::Blt,
                                                             InstructionType::Bge, InstructionType::Bgt};
  if (find(ConditionalBranches.begin(), ConditionalBranches.end(), T->InstrT) == ConditionalBranches.end()) {
    fallthoughSuccessor() = nullptr;
  }

  T->Owner = this;
  auto BranchSuccessor = T->target();
  if (BranchSuccessor != nullptr) {
    BranchSuccessor->appendPredecessor(this);
  }
  Instructions.push_back(move(T));
}

unique_ptr<BasicBlockTerminator> BasicBlock::releaseTerminator() {
  if (Instructions.empty())
    return nullptr;

  auto &LastInstruction = Instructions.back();

  auto Terminator = dynamic_cast<BasicBlockTerminator *>(LastInstruction.get());
  if (Terminator == nullptr)
    return nullptr;

  LastInstruction.release();
  std::unique_ptr<BasicBlockTerminator> TerminatorPtr(Terminator);

  Instructions.pop_back();

  auto Successor = Terminator->target();
  if (Successor != nullptr) {
    auto &FollowerPredecessors = Terminator->target()->Predecessors;
    FollowerPredecessors.erase(remove(FollowerPredecessors.begin(), FollowerPredecessors.end(), this),
                               FollowerPredecessors.end());
  }

  return TerminatorPtr;
}

void BasicBlock::toSSA(SSAContext &SSACtx) {
  // Remove MOVE instructions and update SSA context accordingly.
  for (auto InstIter = Instructions.begin(); InstIter != Instructions.end();) {
    if (auto MovInst = dynamic_cast<MoveInstruction *>(InstIter->get())) {
      if (auto Target = dynamic_cast<Variable *>(MovInst->target())) {
        SSACtx.updateVariable(Target, MovInst->source());
      }
      InstIter = Instructions.erase(InstIter);
      continue;
    }
    (*InstIter)->updateArgs(SSACtx);
    if (auto PhiInst = dynamic_cast<PhiInstruction *>(InstIter->get())) {
      SSACtx.updateVariable(PhiInst->Target, PhiInst);
    }
    InstIter++;
  }
}

void BasicBlock::insertPhiInstruction(unique_ptr<PhiInstruction> Phi) {
  Phi->Owner = this;
  auto PhiMapEntry = PhiInstrMap.find(Phi->Target);
  if (PhiMapEntry == PhiInstrMap.end()) {
    // Basic Block does not contain a Phi node for this variable.
    // Create one and add it to the front of the instruction double ended queue.
    PhiInstrMap[Phi->Target] = Phi.get();
    Instructions.push_front(move(Phi));
    return;
  }
}

vector<Variable *> BasicBlock::getMoveTargets() {
  vector<Variable *> TargetsForPhis;

  for (auto &I : Instructions) {
    if (auto MovInst = dynamic_cast<MoveInstruction *>(I.get())) {
      if (auto Target = dynamic_cast<Variable *>(MovInst->target())) {
        TargetsForPhis.push_back(Target);
      }
    } else if (auto PhiInst = dynamic_cast<PhiInstruction *>(I.get())) {
      TargetsForPhis.push_back(PhiInst->Target);
    }
  }

  return TargetsForPhis;
}

void BasicBlock::updatePhiInst(cs241c::BasicBlock *From, cs241c::Variable *VarToChange, cs241c::Value *NewVal) {
  if (PhiInstrMap.find(VarToChange) == PhiInstrMap.end()) {
    return;
  }
  PhiInstruction *Phi = PhiInstrMap.at(VarToChange);
  auto Index = getPredecessorIndex(From);
  if (Index == -1) {
    throw runtime_error("Invalid PHI update from block: " + string(From->toString()));
  }
  Phi->updateArg(static_cast<unsigned long>(Index), NewVal);
}

vector<BasicBlock *>::difference_type BasicBlock::getPredecessorIndex(cs241c::BasicBlock *Predecessor) {
  auto It = find(Predecessors.begin(), Predecessors.end(), Predecessor);
  if (It == Predecessors.end()) {
    return -1;
  }
  return distance(Predecessors.begin(), It);
}

BasicBlock::iterator BasicBlock::begin() { return Instructions.begin(); }

BasicBlock::iterator BasicBlock::end() { return Instructions.end(); }

string BasicBlock::toString() const { return Name; }
