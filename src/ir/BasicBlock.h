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
  using iterator = std::deque<std::unique_ptr<Instruction>>::iterator;

private:
  std::string Name;
  std::unordered_map<Variable *, PhiInstruction *> PhiInstrMap;

  std::vector<BasicBlock *> Predecessors;
  std::deque<std::unique_ptr<Instruction>> Instructions;

public:
  BasicBlock(std::string Name,
             std::deque<std::unique_ptr<Instruction>> Instructions = {});

  std::vector<BasicBlock *> &predecessors();
  std::deque<std::unique_ptr<Instruction>> &instructions();
  BasicBlockTerminator *terminator();

  void appendPredecessor(BasicBlock *BB);
  void appendInstruction(std::unique_ptr<Instruction> I);

  bool isTerminated();
  void terminate(std::unique_ptr<BasicBlockTerminator> T);
  void toSSA(SSAContext &SSACtx);

  void insertPhiInstruction(BasicBlock *FromBlock,
                            std::unique_ptr<PhiInstruction> Phi);
  void updatePhiInst(BasicBlock *From, Variable *VarToChange, Value *NewVal);

  long getPredecessorIndex(BasicBlock *Predecessor);

  std::vector<std::unique_ptr<PhiInstruction>> genPhis();

  iterator begin();
  iterator end();

  std::string toString() const override;
};
} // namespace cs241c

#endif
