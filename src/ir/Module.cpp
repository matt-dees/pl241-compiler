#include "Module.h"
#include "SSAContext.h"

using namespace cs241c;
using namespace std;

Module::Module(string ModuleName) : Name(move(ModuleName)) {}

Value *Module::globalBase() { return &GlobalBase; }
string Module::getIdentifier() const { return Name; }

vector<unique_ptr<GlobalVariable>> &Module::globals() { return Globals; }

const vector<unique_ptr<GlobalVariable>> &Module::globals() const {
  return Globals;
}

vector<unique_ptr<Function>> &Module::functions() { return Functions; }

const vector<unique_ptr<Function>> &Module::functions() const {
  return Functions;
}

void Module::buildDominatorTree() {
  for (auto &F : functions()) {
    F->buildDominatorTree();
  }
}

void Module::toSSA(IrGenContext &Ctx) {
  for (auto &F : functions()) {
    F->toSSA(Ctx);
  }
}

void Module::writeFunction(std::ofstream &OutFileStream, Function *F) {
  const string FunctionName = F->name();
  for (auto &BB : F->basicBlocks()) {
    writeBasicBlock(OutFileStream, BB.get(), FunctionName);
  }
}

void Module::writeBasicBlock(std::ofstream &OutFileStream, BasicBlock *BB,
                             const string &Title) {
  OutFileStream << "node: {" << endl;

  OutFileStream << "title: "
                << "\"" << BB->toString() << "\"" << endl;
  OutFileStream << "label: \""
                << "[" << Title << "]" << BB->toString() << endl;

  for (auto &Instr : *BB) {
    OutFileStream << Instr->toString() << endl;
  }

  OutFileStream << "\"" << endl;
  OutFileStream << "}" << endl;

  for (auto &Next : BB->terminator()->followingBlocks()) {
    writeEdge(OutFileStream, BB, Next);
  }
}

void Module::writeEdge(std::ofstream &OutFileStream, BasicBlock *Source,
                       BasicBlock *Destination) {
  OutFileStream << "edge: {" << endl;

  OutFileStream << "sourcename: "
                << "\"" << Source->toString() << "\"" << endl;
  OutFileStream << "targetname: "
                << "\"" << Destination->toString() << "\"" << endl;
  OutFileStream << "color: "
                << "blue" << endl;
  OutFileStream << "}" << endl;
}

void Module::writeGraph(std::ofstream &OutFileStream) {
  OutFileStream << "title: "
                << "\"" << getIdentifier() << "\"" << endl;

  for (auto &F : functions()) {
    writeFunction(OutFileStream, F.get());
  }
}
