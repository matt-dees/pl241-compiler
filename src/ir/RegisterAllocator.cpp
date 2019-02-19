#include "RegisterAllocator.h"
#include <iostream>
#include <limits>

using namespace cs241c;
using namespace std;

void InterferenceGraph::writeGraph(ofstream &OutStream) {
  WrittenEdges = {};
  OutStream << "layoutalgorithm: circular\n";
  OutStream << "title: "
            << "\""
            << "Interference Graph"
            << "\"\n";
  writeNodes(OutStream);
}

void InterferenceGraph::addEdges(
    const std::unordered_set<RegAllocValue *> &FromSet, RegAllocValue *To) {
  for (auto Node : FromSet) {
    addEdge(Node, To);
  }
}

void InterferenceGraph::addEdge(RegAllocValue *From, RegAllocValue *To) {
  if (From == To) {
    return;
  }
  if (IG.find(From) == IG.end()) {
    IG[From] = {To};
  } else {
    IG[From].insert(To);
  }
  // Bidirectional graph
  if (IG.find(To) == IG.end()) {
    IG[To] = {From};
  } else {
    IG[To].insert(From);
  }
}

void InterferenceGraph::writeNodes(ofstream &OutStream) {
  for (auto &VertexEdgePair : IG) {
    OutStream << "node: {\n";
    OutStream << "title: "
              << "\"" << VertexEdgePair.first->toString() << "\"\n";
    OutStream << "label: \"" + VertexEdgePair.first->toString() << "\"\n";
    OutStream << "color:red"
              << "\n";
    OutStream << "}\n";
    for (auto Destination : VertexEdgePair.second) {
      writeEdge(OutStream, VertexEdgePair.first, Destination);
    }
  }
}

void InterferenceGraph::writeEdge(ofstream &OutStream, RegAllocValue *From,
                                  RegAllocValue *To) {
  if (WrittenEdges.find(To) != WrittenEdges.end()) {
    if (WrittenEdges.at(To).find(From) != WrittenEdges.at(To).end()) {
      return;
    }
  }
  if (WrittenEdges.find(From) == WrittenEdges.end()) {
    WrittenEdges[From] = {To};
  } else {
    WrittenEdges[From].insert(To);
  }
  OutStream << "edge: {\n";
  OutStream << "sourcename: "
            << "\"" << From->toString() << "\"\n";
  OutStream << "targetname: "
            << "\"" << To->toString() << "\"\n";
  OutStream << "arrowstyle: none"
            << "\n";
  OutStream << "}\n";
}

std::unordered_set<RegAllocValue *>
InterferenceGraph::neighbors(RegAllocValue *RAV) {
  if (IG.find(RAV) == IG.end()) {
    throw logic_error("Value " + RAV->toString() + " not in graph.");
  }
  return IG.at(RAV);
}

std::unordered_set<RegAllocValue *>
InterferenceGraph::removeNode(RegAllocValue *RAV) {
  std::unordered_set<RegAllocValue *> Neighbors = neighbors(RAV);
  IG.erase(RAV);
  for (auto Neighbor : Neighbors) {
    IG[Neighbor].erase(RAV);
  }
  return Neighbors;
}

bool InterferenceGraph::hasNode(RegAllocValue *Node) {
  return IG.find(Node) != IG.end();
}

void InterferenceGraph::addNode(RegAllocValue *Node) { IG[Node] = {}; }

RegisterAllocator::Coloring RegisterAllocator::color(InterferenceGraph IG) {
  if (IG.graph().size() == 0) {
    return {};
  }

  Coloring CurrentColoring = {};
  colorRecur(IG, CurrentColoring);
  return CurrentColoring;
}

namespace {
void addNodeBack(InterferenceGraph &IG, RegAllocValue *Node,
                 std::unordered_set<RegAllocValue *> &Neighbors) {
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
  RegAllocValue *NextNode = chooseNextNodeToColor(IG);
  std::unordered_set<RegAllocValue *> NeighborsOfRemoved =
      IG.removeNode(NextNode);
  colorRecur(IG, CurrentColoring);
  addNodeBack(IG, NextNode, NeighborsOfRemoved);
  assignColor(IG, CurrentColoring, NextNode);
}

RegAllocValue *
RegisterAllocator::getValWithLowestSpillCost(InterferenceGraph &IG) {
  // TODO: Change implementation of this function
  // to use priority queue data structure.
  RegAllocValue *ToReturn = nullptr;
  int32_t CurrentMinSpillCost = std::numeric_limits<int32_t>::max();
  for (auto RAVPair : IG.graph()) {
    int32_t const RAVSpillCost = RAVPair.first->costToSpill();
    if (RAVSpillCost < CurrentMinSpillCost) {
      CurrentMinSpillCost = RAVSpillCost;
      ToReturn = RAVPair.first;
    }
  }
  return ToReturn;
}

RegAllocValue *RegisterAllocator::chooseNextNodeToColor(InterferenceGraph &IG) {
  RegAllocValue *NextNode = nullptr;
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
    cs241c::InterferenceGraph &IG,
    cs241c::RegisterAllocator::Coloring &CurrentColoring,
    cs241c::RegAllocValue *NodeToColor) {
  std::unordered_set<RA_REGISTER> Colors = {R1, R2, R3, R4, R5, R6, R7, R8};
  for (auto Neighbor : IG.neighbors(NodeToColor)) {
    Colors.erase(CurrentColoring[Neighbor->value()]);
  }
  if (Colors.size() == 0) {
    CurrentColoring[NodeToColor->value()] = RA_REGISTER::SPILL;
  } else {
    CurrentColoring[NodeToColor->value()] = *(Colors.begin());
  }
}
