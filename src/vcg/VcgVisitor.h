#ifndef CS241C_VCGVISITOR_H
#define CS241C_VCGVISITOR_H

#include "Instruction.h"
#include <fstream>

namespace cs241c {
class VcgInstructionVisitor : InstructionVisitor {
public:
  VcgInstructionVisitor(const std::fstream &VcgFileStream);
  void visit(NegInstruction *I);
  void visit(AddInstruction *I);
  void visit(SubInstruction *I);
  void visit(MulInstruction *I);
  void visit(DivInstruction *I);
  void visit(CmpInstruction *I);
  void visit(AddaInstruction *I);
  void visit(LoadInstruction *I);
  void visit(StoreInstruction *I);
  void visit(MoveInstruction *I);
  void visit(PhiInstruction *I);
  void visit(RetInstruction *I);
  void visit(EndInstruction *I);
  void visit(BranchInstruction *I);
  void visit(BranchNotEqualInstruction *I);
  void visit(BranchEqualInstruction *I);
  void visit(BranchLessThanEqualInstruction *I);
  void visit(BranchLessThanInstruction *I);
  void visit(BranchGreaterThanEqualInstruction *I);
  void visit(BranchGreaterThanInstruction *I);

private:
  std::fstream VcgFileStream;
};
} // namespace cs241c
#endif // CS241C_VCGVISITOR_H
