#include "BasicBlock.h"
#include <utility>

using namespace cs241c;

BasicBlock::BasicBlock(uint32_t ID,
                       std::vector<std::unique_ptr<Instruction>> Instructions)
    : Instructions(std::move(Instructions)), ID(ID) {}

void BasicBlock::appendInstruction(std::unique_ptr<Instruction> I) {
  I->Owner = this;
  Instructions.push_back(move(I));
}

void BasicBlock::terminate(std::unique_ptr<BasicBlockTerminator> T) {
  T->Owner = this;
  Terminator = move(T);
}

BasicBlock::iterator BasicBlock::begin() { return iterator(this); }
BasicBlock::iterator BasicBlock::end() { return {}; }

BasicBlock::iterator::iterator() : End(true) {}
BasicBlock::iterator::iterator(BasicBlock *BB)
    : BB(BB), InstructionsIt(BB->Instructions.begin()), End(false) {}

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
