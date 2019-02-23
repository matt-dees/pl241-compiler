#ifndef CS241C_IR_BASICBLOCK_H
#define CS241C_IR_BASICBLOCK_H

#include "Instruction.h"
#include "Value.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cs241c {
class BasicBlockTerminator;
class Instruction;
class SSAContext;
class Variable;

enum class BasicBlockAttr : uint8_t {Join = 0x1, If = 0x2, While = 0x4};

class BasicBlock : public Value {
public:
  using iterator = std::vector<std::unique_ptr<Instruction>>::iterator;

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
  BasicBlockAttr Attributes{};

  std::vector<BasicBlock *> Predecessors;
  BasicBlock *FallthroughSuccessor = nullptr;
  std::vector<std::unique_ptr<Instruction>> Instructions;

  std::unordered_map<Variable *, Instruction *> PhiInstrMap;
public:
  BasicBlock(std::string Name,
             std::vector<std::unique_ptr<Instruction>> Instructions = {});

  void addAttribute(BasicBlockAttr Attr);
  bool hasAttribute(BasicBlockAttr Attr);
  void removeAttribute(BasicBlockAttr Attr);

  const std::vector<BasicBlock *> &predecessors() const;

  FallthroughSuccessorProxy fallthoughSuccessor();
  std::vector<BasicBlock *> successors() const;
  void updateSuccessor(BasicBlock *From, BasicBlock *To);

  std::vector<std::unique_ptr<Instruction>> &instructions();
  BasicBlockTerminator *terminator() const;

  void appendPredecessor(BasicBlock *BB);
  void appendInstruction(std::unique_ptr<Instruction> I);

  bool isTerminated();
  void terminate(std::unique_ptr<BasicBlockTerminator> T);
  std::unique_ptr<BasicBlockTerminator> releaseTerminator();

  void insertPhiInstruction(std::unique_ptr<Instruction> Phi);
  void updatePhiInst(BasicBlock *From, Variable *VarToChange, Value *NewVal);

  std::vector<BasicBlock *>::difference_type
  getPredecessorIndex(BasicBlock *Predecessor);

  std::vector<Variable *> getMoveTargets();

  iterator begin();
  iterator end();

  std::string toString() const override;

  friend class ConditionalBlockTerminator;
  friend class BraInstruction;
};
} // namespace cs241c

#endif
