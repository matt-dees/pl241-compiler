#include "Vcg.h"
#include "Filesystem.h"
#include <set>
#include <stdexcept>

using namespace cs241c;
using namespace std;

namespace {

void writeCFGEdge(ofstream &OutFileStream, BasicBlock *Source, BasicBlock *Destination) {
  OutFileStream << "edge: {\n";

  OutFileStream << "sourcename: "
                << "\"" << Source->toString() << "\"\n";
  OutFileStream << "targetname: "
                << "\"" << Destination->toString() << "\"\n";
  OutFileStream << "color: "
                << "black"
                << "\n";
  OutFileStream << "}\n";
}

void writeDomEdge(ofstream &OutFileStream, BasicBlock *Dominatee, BasicBlock *Dominator) {
  OutFileStream << "edge: {\n";

  OutFileStream << "sourcename: "
                << "\"" << Dominatee->toString() << "\"\n";
  OutFileStream << "targetname: "
                << "\"" << Dominator->toString() << "\"\n";
  OutFileStream << "color: "
                << "red"
                << "\n";
  OutFileStream << "}\n";
}

void writeBasicBlock(ofstream &OutFileStream, BasicBlock *BB, const string &Title, Function *F, FunctionAnalyzer &FA) {
  RegisterAllocator::Coloring *Coloring = FA.coloring(F);

  OutFileStream << "node: {\n";

  OutFileStream << "title: "
                << "\"" << BB->toString() << "\"\n";
  OutFileStream << "label: \""
                << "[" << Title << "]" << BB->toString() << "\n";

  for (auto &Instr : *BB) {
    if (Coloring) {
      auto Reg = Coloring->find(Instr.get());
      if (Reg != Coloring->end()) {
        OutFileStream << "R" << Reg->second << ":";
      }
    }
    OutFileStream << Instr->toString() << "\n";
  }

  OutFileStream << "\"\n";
  OutFileStream << "}\n";

  for (auto &Next : BB->successors()) {
    writeCFGEdge(OutFileStream, BB, Next);
  }
}

void writeFunction(ofstream &OutFileStream, Function *F, FunctionAnalyzer &FA, bool WithDomInfo) {
  const string FunctionName = F->name();
  for (auto &BB : F->basicBlocks()) {
    writeBasicBlock(OutFileStream, BB.get(), FunctionName, F, FA);
  }
  if (WithDomInfo) {
    for (auto IDomPair : FA.dominatorTree(F)->IDomMap) {
      if (IDomPair.first != IDomPair.second) {
        writeDomEdge(OutFileStream, IDomPair.first, IDomPair.second);
      }
    }
  }
}

void writeModule(Module &M, FunctionAnalyzer &FA, ofstream &OutFileStream, bool WithDomInfo) {
  OutFileStream << "manhattan_edges: yes\n";
  std::string LayoutType = WithDomInfo ? "hierarchic" : "compilergraph";
  OutFileStream << "layoutalgorithm:" << LayoutType << "\n";
  OutFileStream << "title: "
                << "\"" << M.getIdentifier() << "\"\n";

  for (auto &F : M.functions()) {
    writeFunction(OutFileStream, F.get(), FA, WithDomInfo);
  }
}

}; // namespace

namespace {

struct AIGWriteState {
  std::set<std::pair<InterferenceGraph::IGNode *, InterferenceGraph::IGNode *>> WrittenEdges;
};

void writeIGEdge(AIGWriteState &State, ofstream &OutStream, InterferenceGraph::IGNode *From,
                 InterferenceGraph::IGNode *To) {
  if (State.WrittenEdges.find(std::pair(To, From)) != State.WrittenEdges.end() ||
      State.WrittenEdges.find(std::pair(From, To)) != State.WrittenEdges.end()) {
    return;
  }
  State.WrittenEdges.insert(std::make_pair(From, To));
  OutStream << "edge: {\n";
  OutStream << "sourcename: "
            << "\"" << (*From->Values.begin())->toString() << "\"\n";
  OutStream << "targetname: "
            << "\"" << (*To->Values.begin())->toString() << "\"\n";
  OutStream << "arrowstyle: none"
            << "\n";
  OutStream << "}\n";
}

void writeNodes(AnnotatedIG &AIG, std::ofstream &OutFileStream) {
  AIGWriteState State;
  for (auto &VertexEdgePair : AIG.interferenceGraph().graph()) {
    OutFileStream << "node: {\n";
    OutFileStream << "title: "
                  << "\"" << (*(VertexEdgePair.first->Values.begin()))->toString() << "\"\n";
    OutFileStream << "label: \"";
    for (auto Val : VertexEdgePair.first->Values) {
      OutFileStream << Val->toString() << "\n";
    }
    OutFileStream << "\"";

    OutFileStream << "color:" + AIG.lookupColor((*(VertexEdgePair.first->Values.begin()))) << "\n";
    OutFileStream << "}\n";
    for (auto Destination : VertexEdgePair.second) {
      writeIGEdge(State, OutFileStream, VertexEdgePair.first, Destination);
    }
  }
}

void writeAIG(AnnotatedIG &AIG, std::ofstream &OutFileStream) {
  OutFileStream << "layoutalgorithm: circular\n";
  OutFileStream << "title: "
                << "\""
                << "Colored Interference Graph"
                << "\"\n";
  writeNodes(AIG, OutFileStream);
}

}; // namespace

void VcgWriter::write(AnnotatedIG &AIG, const std::string &OutFilePath) {
  if (fileExists(OutFilePath)) {
    throw runtime_error("VcgGen Error: File already exists: " + OutFilePath);
  }

  ofstream VcgFileStream;
  VcgFileStream.open(OutFilePath);
  VcgFileStream << "graph: {";
  writeAIG(AIG, VcgFileStream);
  VcgFileStream << "}";
  VcgFileStream.close();
}

void VcgWriter::write(Module &M, FunctionAnalyzer &FA, const std::string &OutFilePath, bool WithDomInfo) {
  if (fileExists(OutFilePath)) {
    throw runtime_error("VcgGen Error: File already exists: " + OutFilePath);
  }

  ofstream VcgFileStream;
  VcgFileStream.open(OutFilePath);
  VcgFileStream << "graph: {";
  writeModule(M, FA, VcgFileStream, WithDomInfo);
  VcgFileStream << "}";
  VcgFileStream.close();
}