#include "VcgGen.h"
#include "Filesystem.h"

#include <stdexcept>

using namespace cs241c;
using namespace std;

VcgGen::VcgGen(const Module *InputModule) : InputModule(InputModule) {}

void VcgGen::generate(const string &OutFilePath) {
  if (fileExists(OutFilePath)) {
    throw runtime_error("VcgGen Error: File already exists: " + OutFilePath);
  }

  VcgFileStream.open(OutFilePath);
  writeGraph();
  VcgFileStream.close();
}

void VcgGen::writeGraph() {
  VcgFileStream << "graph: {" << endl;
  VcgFileStream << "title: "
                << "\"" << InputModule->getIdentifier() << "\"" << endl;

  writeProperties();

  for (auto &F : InputModule->functions()) {
    writeFunction(F.get());
  }

  VcgFileStream << "}";
}

void VcgGen::writeFunction(Function *F) {
  const string FunctionName = F->name();
  for (auto &BB : F->basicBlocks()) {
    writeBasicBlock(BB.get(), FunctionName);
  }
}

void VcgGen::writeBasicBlock(BasicBlock *BB, const string &Title) {
  VcgFileStream << "node: {" << endl;

  VcgFileStream << "title: "
                << "\"" << BB->toString() << "\"" << endl;
  VcgFileStream << "label: \""
                << "[" << Title << "]" << BB->toString() << endl;

  // Note: Assumes terminating instruction is in Instructions vector

  for (auto &Instr : *BB) {
    VcgFileStream << Instr->toString() << endl;
  }

  VcgFileStream << "\"" << endl;
  VcgFileStream << "}" << endl;

  for (auto &Next : BB->terminator()->followingBlocks()) {
    writeEdge(BB, Next);
  }
}

void VcgGen::writeEdge(BasicBlock *Source, BasicBlock *Destination) {
  VcgFileStream << "edge: {" << endl;

  VcgFileStream << "sourcename: "
                << "\"" << Source->toString() << "\"" << endl;
  VcgFileStream << "targetname: "
                << "\"" << Destination->toString() << "\"" << endl;
  VcgFileStream << "color: "
                << "blue" << endl;
  VcgFileStream << "}" << endl;
}

void VcgGen::writeProperties() {
  VcgFileStream << "layoutalgorithm: dfs" << endl;
  VcgFileStream << "manhattan_edges: yes" << endl;
}
