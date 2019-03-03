#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;
using namespace std;

Module::Module(string ModuleName) : Name(move(ModuleName)) {}

Value *Module::globalBase() { return &GlobalBase; }
string Module::getIdentifier() const { return Name; }

vector<unique_ptr<GlobalVariable>> &Module::globals() { return Globals; }

const vector<unique_ptr<GlobalVariable>> &Module::globals() const { return Globals; }

vector<unique_ptr<Function>> &Module::functions() { return Functions; }

const vector<unique_ptr<Function>> &Module::functions() const { return Functions; }

void Module::writeFunction(ofstream &OutFileStream, Function *F) {
  const string FunctionName = F->name();
  for (auto &BB : F->basicBlocks()) {
    writeBasicBlock(OutFileStream, BB.get(), FunctionName, F);
  }
}

void Module::writeBasicBlock(ofstream &OutFileStream, BasicBlock *BB, const string &Title, Function *F) {
  RegisterAllocator::Coloring *Coloring = nullptr;
  if (FA) {
    Coloring = FA->coloring(F);
  }

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
    writeEdge(OutFileStream, BB, Next);
  }
}

void Module::writeEdge(ofstream &OutFileStream, BasicBlock *Source, BasicBlock *Destination) {
  OutFileStream << "edge: {\n";

  OutFileStream << "sourcename: "
                << "\"" << Source->toString() << "\"\n";
  OutFileStream << "targetname: "
                << "\"" << Destination->toString() << "\"\n";
  OutFileStream << "color: "
                << "blue\n";
  OutFileStream << "}\n";
}

void Module::writeGraph(ofstream &OutFileStream) {
  OutFileStream << "manhattan_edges: yes\n";
  OutFileStream << "layoutalgorithm: compilergraph\n";
  OutFileStream << "title: "
                << "\"" << getIdentifier() << "\"\n";

  for (auto &F : functions()) {
    writeFunction(OutFileStream, F.get());
  }
}