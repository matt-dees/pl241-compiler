#ifndef CS241C_IR_BASICBLOCK_H
#define CS241C_IR_BASICBLOCK_H

#include "Instruction.h"
#include "SSAContext.h"
#include "Value.h"
#include <cstdint>
#include <deque>
#include <memory>

namespace cs241c {
class BasicBlockTerminator;
class Instruction;
class PhiInstruction;

class BasicBlock : public Value {
public:
  class iterator {
    BasicBlock *BB;
    std::deque<std::unique_ptr<Instruction>>::iterator InstructionsIt;
    bool End;

  public:
    iterator(BasicBlock *BB, bool End = false);

    iterator &operator++();
    Instruction *operator*();
    bool operator==(const iterator &b) const;
    bool operator!=(const iterator &b) const;
  };

private:
  std::string Name;
  std::unordered_map<Variable *, Instruction *> PhiInstrMap;

public:
  std::vector<BasicBlock *> Predecessors;
  std::deque<std::unique_ptr<Instruction>> Instructions;
  std::unique_ptr<BasicBlockTerminator> Terminator;

  BasicBlock(std::string Name,
             std::deque<std::unique_ptr<Instruction>> Instructions = {});

  void appendPredecessor(BasicBlock *BB);
  void appendInstruction(std::unique_ptr<Instruction> I);

  bool isTerminated();
  void terminate(std::unique_ptr<BasicBlockTerminator> T);
  void toSSA(SSAContext &SSACtx);

  void insertPhiInstruction(BasicBlock *FromBlock,
                            std::unique_ptr<PhiInstruction> Phi);

  std::vector<std::unique_ptr<PhiInstruction>> genPhis();

  iterator begin();
  iterator end();

  std::string toString() const override;
};
} // namespace cs241c

#endif
