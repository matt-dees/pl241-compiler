#ifndef CS241C_IR_BASICBLOCK_H
#define CS241C_IR_BASICBLOCK_H

#include "Instruction.h"
#include "Value.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace cs241c {
class BasicBlockTerminator;
class Instruction;

class BasicBlock : public Value {
public:
  class iterator {
    BasicBlock *BB;
    std::vector<std::unique_ptr<Instruction>>::iterator InstructionsIt;
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

public:
  std::vector<std::unique_ptr<ConstantValue>> Constants;
  std::vector<std::unique_ptr<Instruction>> Instructions;
  std::unique_ptr<BasicBlockTerminator> Terminator;

  BasicBlock(std::string Name,
             std::vector<std::unique_ptr<Instruction>> Instructions = {});

  void appendInstruction(std::unique_ptr<Instruction> I);

  bool isTerminated();
  void terminate(std::unique_ptr<BasicBlockTerminator> T);

  iterator begin();
  iterator end();

  std::string toString() const override;
};
} // namespace cs241c

#endif
