#include "VcgGen.h"
#include <filesystem>

#include <stdexcept>

using namespace cs241c;
namespace fs = std::filesystem;

VcgGen::VcgGen(const Module *InputModule) : InputModule(InputModule) {}

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
  VcgFileStream << "graph: {" << std::endl;
  VcgFileStream << "title: "
                << "\"" << InputModule->getIdentifier() << "\"" << std::endl;

  writeProperties();

  for (auto &F : InputModule->Functions) {
    writeFunction(F.get());
  }

  VcgFileStream << "}";
}

void VcgGen::writeFunction(Function *F) {
  const std::string FunctionName = F->Name;
  for (auto &BB : F->BasicBlocks) {
    writeBasicBlock(BB.get(), FunctionName);
  }
}

void VcgGen::writeBasicBlock(BasicBlock *BB, const std::string &Title) {
  VcgFileStream << "node: {" << std::endl;

  VcgFileStream << "title: "
                << "\"" << BB->ID << "\"" << std::endl;
  VcgFileStream << "label: \"" << BB->ID << std::endl;

  // Note: Assumes terminating instruction is in Instructions vector

  for (auto &I : BB->Instructions) {
    VcgFileStream << I->toString() << std::endl;
  }

  VcgFileStream << "\"" << std::endl;
  VcgFileStream << "}" << std::endl;

  for (auto &Next : BB->Terminator.followingBlocks()) {
    writeEdge(BB, Next);
  }
}

void VcgGen::writeEdge(BasicBlock *Source, BasicBlock *Destination) {
  VcgFileStream << "edge: {" << std::endl;

  VcgFileStream << "sourcename: "
                << "\"" << Source->ID << "\"" << std::endl;
  VcgFileStream << "targetname: "
                << "\"" << Destination->ID << "\"" << std::endl;
  VcgFileStream << "color: "
                << "blue" << std::endl;
  VcgFileStream << "}" << std::endl;
}

void VcgGen::writeProperties() {
  VcgFileStream << "layoutalgorithm: dfs" << std::endl;
  VcgFileStream << "manhattan_edges: yes" << std::endl;
  VcgFileStream << "smanhattan_edges: yes" << std::endl;
}