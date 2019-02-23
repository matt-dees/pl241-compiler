#include "RegisterAllocator.h"
#include <iostream>
#include <limits>

using namespace cs241c;
using namespace std;

RegisterAllocator::Coloring RegisterAllocator::color(InterferenceGraph IG) {
  if (IG.graph().size() == 0) {
    return {};
  }

  Coloring CurrentColoring = {};
  colorRecur(IG, CurrentColoring);
  for (auto Node : IG.coalescedNodes()) {
    CurrentColoring[Node.first] = CurrentColoring.at(Node.second);
  }
  return CurrentColoring;
}

namespace {
void addNodeBack(InterferenceGraph &IG, Value *Node,
                 std::unordered_set<Value *> &Neighbors) {
  IG.addNode(Node);
  for (auto Neighbor : Neighbors) {
    if (IG.hasNode(Neighbor)) {
      IG.addEdge(Node, Neighbor);
    }
  }
}
}; // namespace

void RegisterAllocator::colorRecur(
    InterferenceGraph &IG, RegisterAllocator::Coloring &CurrentColoring) {
  if (IG.graph().size() == 1) {
    assignColor(IG, CurrentColoring, IG.graph().begin()->first);
    return;
  }
  Value *NextNode = chooseNextNodeToColor(IG);
  std::unordered_set<Value *> NeighborsOfRemoved = IG.removeNode(NextNode);
  colorRecur(IG, CurrentColoring);
  addNodeBack(IG, NextNode, NeighborsOfRemoved);
  assignColor(IG, CurrentColoring, NextNode);
}

Value *RegisterAllocator::getValWithLowestSpillCost(InterferenceGraph &IG) {
  // TODO: Change implementation of this function
  // to use priority queue data structure.
  Value *ToReturn = nullptr;
  int32_t CurrentMinSpillCost = std::numeric_limits<int32_t>::max();
  for (auto RAVPair : IG.graph()) {
    int32_t const RAVSpillCost = IG.heuristicData(RAVPair.first).spillCost();
    if (RAVSpillCost < CurrentMinSpillCost) {
      CurrentMinSpillCost = RAVSpillCost;
      ToReturn = RAVPair.first;
    }
  }
  return ToReturn;
}

Value *RegisterAllocator::chooseNextNodeToColor(InterferenceGraph &IG) {
  Value *NextNode = nullptr;
  for (auto RAVPair : IG.graph()) {
    if (IG.neighbors(RAVPair.first).size() < NUM_REGISTERS) {
      NextNode = RAVPair.first;
      break;
    }
  }

  if (!NextNode) {
    NextNode = getValWithLowestSpillCost(IG);
  }
  return NextNode;
}

void RegisterAllocator::assignColor(
    InterferenceGraph &IG, RegisterAllocator::Coloring &CurrentColoring,
    Value *NodeToColor) {
  std::unordered_set<RA_REGISTER> RemainingColors = {R1, R2, R3, R4,
                                                     R5, R6, R7, R8};
  for (auto Neighbor : IG.neighbors(NodeToColor)) {
    RemainingColors.erase(CurrentColoring[Neighbor]);
  }
  CurrentColoring[NodeToColor] =
      RemainingColors.empty() ? RA_REGISTER::SPILL : *(RemainingColors.begin());
}

void AnnotatedIG::writeGraph(std::ofstream &OutFileStream) {
  IG->reset();
  OutFileStream << "layoutalgorithm: circular\n";
  OutFileStream << "title: "
                << "\""
                << "Colored Interference Graph"
                << "\"\n";
  writeNodes(OutFileStream);
}

std::string AnnotatedIG::lookupColor(Value *RAV) {
  if (C->find(RAV) == C->end()) {
    return "white";
  }
  switch (C->at(RAV)) {
  case RegisterAllocator::RA_REGISTER::SPILL:
    return "darkgrey";
  case RegisterAllocator::RA_REGISTER::R1:
    return "lightblue";
  case RegisterAllocator::RA_REGISTER::R2:
    return "lightcyan";
  case RegisterAllocator::RA_REGISTER::R3:
    return "lightgreen";
  case RegisterAllocator::RA_REGISTER::R4:
    return "lightmagenta";
  case RegisterAllocator::RA_REGISTER::R5:
    return "lightred";
  case RegisterAllocator::RA_REGISTER::R6:
    return "lightyellow";
  case RegisterAllocator::RA_REGISTER::R7:
    return "pink";
  case RegisterAllocator::RA_REGISTER::R8:
    return "yellowgreen";
  }
  return "white";
}

void AnnotatedIG::writeNodes(std::ofstream &OutFileStream) {
  for (auto &VertexEdgePair : IG->graph()) {
    OutFileStream << "node: {\n";
    OutFileStream << "title: "
                  << "\"" << VertexEdgePair.first->toString() << "\"\n";
    OutFileStream << "label: \"" + VertexEdgePair.first->toString() << "\"\n";
    OutFileStream << "color:" + lookupColor(VertexEdgePair.first) << "\n";
    OutFileStream << "}\n";
    for (auto Destination : VertexEdgePair.second) {
      IG->writeEdge(OutFileStream, VertexEdgePair.first, Destination);
    }
  }
}
