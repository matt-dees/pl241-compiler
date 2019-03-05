#include "FunctionAnalyzer.h"

using namespace cs241c;
DominatorTree *FunctionAnalyzer::dominatorTree(Function *F) {
  if (DTMap.find(F) == DTMap.end()) {
    return nullptr;
  }
  return DTMap.at(F).get();
}

InterferenceGraph *FunctionAnalyzer::interferenceGraph(Function *F) {
  if (IGMap.find(F) == IGMap.end()) {
    return nullptr;
  }
  return IGMap.at(F).get();
}

RegisterAllocator::Coloring *FunctionAnalyzer::coloring(Function *F) {
  if (RAColoring.find(F) == RAColoring.end()) {
    return nullptr;
  }
  return RAColoring.at(F).get();
}

void FunctionAnalyzer::buildDominatorTree(Function *F) {
  std::unique_ptr<DominatorTree> DT = std::make_unique<DominatorTree>();
  DT->buildDominatorTree(*F);
  DTMap[F] = std::move(DT);
}

void FunctionAnalyzer::buildInterferenceGraph(Function *F) {
  IGBuilder IGB(F, dominatorTree(F));
  IGB.buildInterferenceGraph();
  IGMap[F] = std::make_unique<InterferenceGraph>(std::move(IGB.interferenceGraph()));
}

void FunctionAnalyzer::runDominanceAnalytics(Module *M) {
  for (auto &F : M->functions()) {
    buildDominatorTree(F.get());
  }
}

void FunctionAnalyzer::runRegisterAllocation(Module *M) {
  for (auto &F : M->functions()) {
    buildInterferenceGraph(F.get());
    RegisterAllocator RA;
    auto InterferenceGraph = interferenceGraph(F.get())->graph();
    auto Coloring = RA.color(InterferenceGraph, *F);
    RAColoring[F.get()] = std::make_unique<RegisterAllocator::Coloring>(Coloring);
  }
}

bool FunctionAnalyzer::isValueSpilled(Function *F, Value *Val) {
  RegisterAllocator::Coloring *Coloring = coloring(F);
  if (Coloring->find(Val) == Coloring->end()) {
    return true;
  }
  RegisterAllocator::VirtualRegister Reg = Coloring->at(Val);
  return isRegisterSpilled(Reg);
}

bool FunctionAnalyzer::isRegisterSpilled(RegisterAllocator::VirtualRegister Reg) {
  return Reg < 1 || Reg > RegisterAllocator::NUM_REGISTERS;
}