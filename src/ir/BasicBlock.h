#ifndef CS241C_IR_BASICBLOCK_H
#define CS241C_IR_BASICBLOCK_H

#include "Instruction.h"
#include "Value.h"
#include <cstdint>
#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cs241c {
class BasicBlockTerminator;
class Instruction;
class SSAContext;
class Variable;

class BasicBlock : public Value {
public:
  using iterator = std::deque<std::unique_ptr<Instruction>>::iterator;

  class FallthroughSuccessorProxy {
    BasicBlock *BB;
    FallthroughSuccessorProxy(BasicBlock *);

  public:
    operator BasicBlock *() const;
    FallthroughSuccessorProxy &operator=(BasicBlock *Other);

    friend class BasicBlock;
  };
  friend class FallthroughSuccessorProxy;

private:
  std::string Name;
  std::unordered_map<Variable *, Instruction *> PhiInstrMap;

  std::vector<BasicBlock *> Predecessors;
  BasicBlock *FallthroughSuccessor = nullptr;
  std::deque<std::unique_ptr<Instruction>> Instructions;

public:
  BasicBlock(std::string Name, std::deque<std::unique_ptr<Instruction>> Instructions = {});

  const std::vector<BasicBlock *> &predecessors() const;

  FallthroughSuccessorProxy fallthoughSuccessor();
  std::vector<BasicBlock *> successors() const;
  void updateSuccessor(BasicBlock *From, BasicBlock *To);

  std::deque<std::unique_ptr<Instruction>> &instructions();
  BasicBlockTerminator *terminator() const;

  void appendPredecessor(BasicBlock *BB);
  void appendInstruction(std::unique_ptr<Instruction> I);

  bool isTerminated();
  void terminate(std::unique_ptr<BasicBlockTerminator> T);
  std::unique_ptr<BasicBlockTerminator> releaseTerminator();
  void toSSA(SSAContext &SSACtx);

  void insertPhiInstruction(std::unique_ptr<Instruction> Phi);
  void updatePhiInst(BasicBlock *From, Variable *VarToChange, Value *NewVal);

  std::vector<BasicBlock *>::difference_type getPredecessorIndex(BasicBlock *Predecessor);

  std::vector<Variable *> getMoveTargets();

  iterator begin();
  iterator end();

  std::string toString() const override;

  friend class ConditionalBlockTerminator;
  friend class BraInstruction;
};
} // namespace cs241c

#endif
