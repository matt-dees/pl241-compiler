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

void BasicBlock::toSSA(SSAContext &SSACtx) {

  // Need to update SSACtx with Phi nodes.
  // Note: Could also store Variable * inside Phi instruction and then
  // check Phi instructions similarly to Move instructions inside of the loops.
  for (auto PhiMapEntry : PhiInstrMap) {
    SSACtx.updateVariable(PhiMapEntry.first, PhiMapEntry.second);
  }
  for (auto InstIter = Instructions.begin(); InstIter != Instructions.end();) {
    if (auto MovInst = dynamic_cast<MoveInstruction *>(InstIter->get())) {
      if (auto Target = dynamic_cast<Variable *>(MovInst->Target)) {
        SSACtx.updateVariable(Target, MovInst->Source);
      }
      InstIter = Instructions.erase(InstIter);
      continue;
    }
    (*InstIter)->argsToSSA(SSACtx);
    InstIter++;
  }
}

void BasicBlock::insertPhiInstruction(BasicBlock *FromBlock,
                                      std::unique_ptr<PhiInstruction> Phi) {
  auto PhiMapEntry = PhiInstrMap.find(Phi->Target);
  if (PhiMapEntry == PhiInstrMap.end()) {
    // Basic Block does not contain a Phi node for this variable.
    // Create one and add it to the front of the instruction double ended queue.
    PhiInstrMap[Phi->Target] = Phi.get();
    Instructions.push_front(std::move(Phi));
    return;
  }
  // Basic Block does contain Phi node for this variable. Update args
  // accordingly.
  auto It = std::find(Predecessors.begin(), Predecessors.end(), FromBlock);
  if (It == Predecessors.end()) {
    throw std::runtime_error("Invalid PHI insertion from block: " +
                             std::string(FromBlock->toString()));
  }
  long Index = std::distance(Predecessors.begin(), It);
  (*PhiMapEntry)
      .second->updateArg(static_cast<unsigned long>(Index),
                         Phi->getArguments().at(Index));
}

std::vector<std::unique_ptr<PhiInstruction>> BasicBlock::genPhis() {
  std::vector<std::unique_ptr<PhiInstruction>> PhisToPropagate;

  for (auto &I : Instructions) {
    if (auto MovInst = dynamic_cast<MoveInstruction *>(I.get())) {
      if (auto Target = dynamic_cast<Variable *>(MovInst->Target)) {
        PhisToPropagate.push_back(
            std::make_unique<PhiInstruction>(0, Target, Target, Target));
      }
    } else if (auto PhiInst = dynamic_cast<PhiInstruction *>(I.get())) {
      PhisToPropagate.push_back(std::make_unique<PhiInstruction>(
          0, PhiInst->Target, PhiInst->Target, PhiInst->Target));
    }
  }

  return PhisToPropagate;
}