#ifndef CS241C_VCGGEN_H
#define CS241C_VCGGEN_H

#include "Module.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

namespace cs241c {
class VcgGen : InstructionVisitor {
public:
  explicit VcgGen(const Module &InputModule);

  // Generates .vcg file that serves as input to the VCG program.
  // Throws exception if unable to generate file correctly.
  void generate(const std::string &OutFilePath);

private:
  void writeGraph();
  void writeBasicBlock(const BasicBlock &BB, const std::string &Title);
  void writeEdge(BasicBlock *Source, BasicBlock *Destination);
  void writeProperties();
  void writeFunction(const Function &F);

  void visitNormalInstruction(Instruction *I);
  void visitTerminatingInstruction(BasicBlockTerminator *I);
  void visitBranchInstruction(BasicBlockTerminator *I);

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

  std::ofstream VcgFileStream;
  const Module InputModule;
};
} // namespace cs241c
#endif // CS241C_VCGGEN_H
