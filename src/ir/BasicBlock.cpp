#include "BasicBlock.h"
#include <algorithm>
#include <cassert>
#include <utility>

using namespace cs241c;

BasicBlock::BasicBlock(std::string Name,
                       std::deque<std::unique_ptr<Instruction>> Instructions)
    : Name(move(Name)), Predecessors({}),
      Instructions(std::move(Instructions)) {}

void BasicBlock::appendInstruction(std::unique_ptr<Instruction> I) {
  I->Owner = this;
  Instructions.push_back(move(I));
}

void BasicBlock::appendPredecessor(cs241c::BasicBlock *BB) {
  Predecessors.push_back(BB);
}
bool BasicBlock::isTerminated() { return Terminator != nullptr; }

void BasicBlock::terminate(std::unique_ptr<BasicBlockTerminator> T) {
  T->Owner = this;
  Terminator = move(T);
  for (auto BB : Terminator->followingBlocks()) {
    BB->appendPredecessor(this);
  }
}

std::string BasicBlock::toString() const { return Name; }

BasicBlock::iterator BasicBlock::begin() { return iterator(this); }
BasicBlock::iterator BasicBlock::end() { return iterator(this, true); }

BasicBlock::iterator::iterator(BasicBlock *BB, bool End)
    : BB(BB),
      InstructionsIt(End ? BB->Instructions.end() : BB->Instructions.begin()),
      End(End) {}

BasicBlock::iterator &BasicBlock::iterator::operator++() {
  if (End) {
    return *this;
  }

  if (InstructionsIt == BB->Instructions.end()) {
    End = true;
    return *this;
  }

  InstructionsIt++;
  return *this;
}

Instruction *BasicBlock::iterator::operator*() {
  if (InstructionsIt == BB->Instructions.end()) {
    return BB->Terminator.get();
  } else {
    return InstructionsIt->get();
  }
}

bool BasicBlock::iterator::operator==(const BasicBlock::iterator &it) const {
  bool InstrEq = (InstructionsIt == it.InstructionsIt);
  bool EndEq = (End == it.End);
  return InstrEq && EndEq;
}

bool BasicBlock::iterator::operator!=(const BasicBlock::iterator &it) const {
  return !(*this == it);
}

void BasicBlock::toSSA(
    SSAContext &SSACtx,
    std::unordered_multimap<BasicBlock *, BasicBlock *>::iterator DF) {
  for (auto InstIter = Instructions.begin(); InstIter != Instructions.end();
       InstIter++) {
    if (auto MovInst = dynamic_cast<MoveInstruction *>(InstIter->get())) {
      if (auto Target = dynamic_cast<Variable *>(MovInst->Target)) {
        SSACtx.updateVariable(Target, MovInst->Source);
      }
      Instructions.erase(InstIter);
      continue;
    }
    for (auto &Arg : (*InstIter)->getArguments()) {
      if (auto Var = dynamic_cast<Variable *>(Arg)) {
        Arg = SSACtx.lookupVariable(Var);
      }
    }
  }
}

void BasicBlock::insertPhiInstruction(Variable *Var, Value *Val, int Id,
                                      BasicBlock *Inserter) {
  auto PhiMapEntry = PhiInstrMap.find(Var);
  if (PhiMapEntry == PhiInstrMap.end()) {
    // Basic Block does not contain a Phi node for this variable.
    // Create one and add it to the front of the instruction double ended queue.
    auto Phi = std::make_unique<PhiInstruction>(Id, Val, Val);
    Instructions.push_front(std::move(Phi));
    PhiInstrMap.at(Var) = Phi.get();
    return;
  }
  // Basic Block does contain Phi node for this variable. Update args
  // accordingly.
  auto It = std::find(Predecessors.begin(), Predecessors.end(), Inserter);
  if (It == Predecessors.end()) {
    assert(false);
  }
  long Index = std::distance(Predecessors.begin(), It);
  (*PhiMapEntry).second->getArguments().at(static_cast<unsigned long>(Index)) =
      Val;
}