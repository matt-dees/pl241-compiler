#include "VcgGen.h"
#include <filesystem>

#include <stdexcept>

using namespace cs241c;
namespace fs = std::filesystem;

VcgGen::VcgGen(const Module &InputModule) : InputModule(InputModule) {}

void VcgGen::generate(const std::string &OutFilePath) {
  if (fs::exists(OutFilePath)) {
    throw std::runtime_error("VcgGen Error: File already exists: " +
                             OutFilePath);
  }

  VcgFileStream.open(OutFilePath);
  writeGraph();
  VcgFileStream.close();
}

void VcgGen::writeGraph() {
  VcgFileStream << "graph : {" << std::endl;
  VcgFileStream << "title : "
                << "\"" << InputModule.getIdentifier() << "\"" << std::endl;

  writeProperties();

  for (auto &Function : InputModule.Functions) {
    const std::string currentFunctionName = Function.Name;
  }
  VcgFileStream << "}";
}

void VcgGen::writeFunction(const Function &F) {
  const std::string FunctionName = F.Name;
  for (auto &BB : F.BasicBlocks) {
    writeBasicBlock(BB, FunctionName);
  }
}

void VcgGen::writeBasicBlock(const cs241c::BasicBlock &BB,
                             const std::string &Title) {
  VcgFileStream << "node: {" << std::endl;
  VcgFileStream << "title: "
                << "\"" << Title << "\"" << std::endl;
  VcgFileStream << "label: ";
  VcgFileStream << "\"";

  for (auto &I : BB.Instructions) {
    VcgFileStream << I->toString() << std::endl;
  }

  VcgFileStream << BB.Terminator.toString() << std::endl;
  VcgFileStream << "\"" << std::endl;
  VcgFileStream << "}" << std::endl;

  if (dynamic_cast<>)
}

void VcgGen::visitNormalInstruction(Instruction *I) {
  VcgFileStream << I->toString() << std::endl;
}

void VcgGen::visitTerminatingInstruction(BasicBlockTerminator *I) {
  VcgFileStream << I->toString() << std::endl;
  VcgFileStream << "\"" << std::endl;
  VcgFileStream << "}" << std::endl;
}

void VcgGen::visitBranchInstruction(cs241c::BasicBlockTerminator *I) {
  visitTerminatingInstruction(I);
  writeEdge()
}

void VcgGen::visit(NegInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(AddInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(SubInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(MulInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(DivInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(CmpInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(AddaInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(LoadInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(StoreInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(MoveInstruction *I) { visitNormalInstruction(I); }
void VcgGen::visit(PhiInstruction *I) { visitNormalInstruction(I); }
