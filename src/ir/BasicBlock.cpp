#include "BasicBlock.h"
#include <algorithm>
#include <cassert>
#include <utility>

using namespace cs241c;

BasicBlock::BasicBlock(std::string Name,
                       std::deque<std::unique_ptr<Instruction>> Instructions)
    : Name(move(Name)), Predecessors({}),
      Instructions(std::move(Instructions)) {}

std::vector<BasicBlock *> &BasicBlock::predecessors() { return Predecessors; }

std::deque<std::unique_ptr<Instruction>> &BasicBlock::instructions() {
  return Instructions;
}

BasicBlockTerminator *BasicBlock::terminator() {
  return dynamic_cast<BasicBlockTerminator *>(Instructions.back().get());
}

void BasicBlock::appendInstruction(std::unique_ptr<Instruction> I) {
  I->Owner = this;
  Instructions.push_back(move(I));
}

void BasicBlock::appendPredecessor(BasicBlock *BB) {
  Predecessors.push_back(BB);
}
bool BasicBlock::isTerminated() {
  return !Instructions.empty() &&
         dynamic_cast<BasicBlockTerminator *>(Instructions.back().get());
}

void BasicBlock::terminate(std::unique_ptr<BasicBlockTerminator> T) {
  T->Owner = this;
  for (auto BB : T->followingBlocks()) {
    BB->appendPredecessor(this);
  }
  Instructions.push_back(move(T));
}

void BasicBlock::toSSA(SSAContext &SSACtx) {
  // Remove MOVE instructions and update SSA context accordingly.
  for (auto InstIter = Instructions.begin(); InstIter != Instructions.end();) {
    if (auto MovInst = dynamic_cast<MoveInstruction *>(InstIter->get())) {
      if (auto Target = dynamic_cast<Variable *>(MovInst->Target)) {
        SSACtx.updateVariable(Target, MovInst->Source);
      }
      InstIter = Instructions.erase(InstIter);
      continue;
    }
    (*InstIter)->argsToSSA(SSACtx);
    if (auto PhiInst = dynamic_cast<PhiInstruction *>(InstIter->get())) {
      SSACtx.updateVariable(PhiInst->Target, PhiInst);
    }
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
  long Index = getPredecessorIndex(FromBlock);
  if (Index == -1) {
    throw std::runtime_error("Invalid PHI insertion from block: " +
                             std::string(FromBlock->toString()));
  }
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

void BasicBlock::updatePhiInst(cs241c::BasicBlock *From,
                               cs241c::Variable *VarToChange,
                               cs241c::Value *NewVal) {
  PhiInstruction *Phi = PhiInstrMap.at(VarToChange);
  long Index = getPredecessorIndex(From);
  if (Index == -1) {
    throw std::runtime_error("Invalid PHI update from block: " +
                             std::string(From->toString()));
  }
  Phi->updateArg(static_cast<unsigned long>(Index), NewVal);
}

long BasicBlock::getPredecessorIndex(cs241c::BasicBlock *Predecessor) {
  auto It = std::find(Predecessors.begin(), Predecessors.end(), Predecessor);
  if (It == Predecessors.end()) {
    return -1;
  }
  return std::distance(Predecessors.begin(), It);
}

BasicBlock::iterator BasicBlock::begin() { return Instructions.begin(); }

BasicBlock::iterator BasicBlock::end() { return Instructions.end(); }

std::string BasicBlock::toString() const { return Name; }
