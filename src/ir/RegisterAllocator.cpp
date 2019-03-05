#include "RegisterAllocator.h"
#include <iostream>
#include <limits>
#include <set>
#include <stack>
#include <utility>
using namespace cs241c;
using namespace std;

RegisterAllocator::Coloring RegisterAllocator::color(InterferenceGraph::Graph IG) {
  if (IG.size() == 0) {
    return {};
  }

  std::unordered_map<InterferenceGraph::IGNode *, VirtualRegister> IGNodeColoring = {};
  color(IG, IGNodeColoring);

  Coloring ToReturn;
  for (auto ColorPair : IGNodeColoring) {
    for (auto Value : ColorPair.first->Values) {
      ToReturn[Value] = ColorPair.second;
    }
  }
  return ToReturn;
}

namespace {
void addNodeBack(InterferenceGraph::Graph &IG, InterferenceGraph::IGNode *Node,
                 std::unordered_set<InterferenceGraph::IGNode *> &Neighbors) {
  IG[Node] = Neighbors;
  for (auto Neighbor : Neighbors) {
    if (IG.find(Neighbor) != IG.end()) {
      IG.at(Neighbor).insert(Node);
    }
  }
}
void removeNode(InterferenceGraph::Graph &IG, InterferenceGraph::IGNode *Node) {
  for (auto Neighbor : IG.at(Node)) {
    IG.at(Neighbor).erase(Node);
  }
  IG.erase(Node);
}
}; // namespace

void RegisterAllocator::color(InterferenceGraph::Graph &IG,
                              std::unordered_map<InterferenceGraph::IGNode *, VirtualRegister> &CurrentColoring) {
  stack<pair<InterferenceGraph::IGNode *, unordered_set<InterferenceGraph::IGNode *>>> RemovedNodes;

  while (IG.size() > 1) {
    InterferenceGraph::IGNode *NextNode = chooseNextNodeToColor(IG);
    std::unordered_set<InterferenceGraph::IGNode *> NeighborsOfRemoved = IG.at(NextNode);
    removeNode(IG, NextNode);
    RemovedNodes.push({NextNode, move(NeighborsOfRemoved)});
  }

  assignColor(IG, CurrentColoring, IG.begin()->first);

  while (!RemovedNodes.empty()) {
    auto &NextNode = RemovedNodes.top();
    addNodeBack(IG, NextNode.first, NextNode.second);
    assignColor(IG, CurrentColoring, NextNode.first);
    RemovedNodes.pop();
  }
}

InterferenceGraph::IGNode *RegisterAllocator::getNodeWithLowestSpillCost(InterferenceGraph::Graph &IG) {
  // TODO: Change implementation of this function
  // to use priority queue data structure.
  InterferenceGraph::IGNode *ToReturn = nullptr;
  int32_t CurrentMinSpillCost = std::numeric_limits<int32_t>::max();
  for (auto NodeToNeighborsPair : IG) {
    int32_t const SpillCost = NodeToNeighborsPair.first->spillCost();
    if (SpillCost < CurrentMinSpillCost) {
      CurrentMinSpillCost = SpillCost;
      ToReturn = NodeToNeighborsPair.first;
    }
  }
  return ToReturn;
}

InterferenceGraph::IGNode *RegisterAllocator::chooseNextNodeToColor(InterferenceGraph::Graph &IG) {
  InterferenceGraph::IGNode *NextNode = nullptr;
  for (auto NodeToNeighborsPair : IG) {
    if (NodeToNeighborsPair.second.size() < NUM_REGISTERS) {
      NextNode = NodeToNeighborsPair.first;
      break;
    }
  }

  if (!NextNode) {
    NextNode = getNodeWithLowestSpillCost(IG);
  }
  return NextNode;
}

void RegisterAllocator::assignColor(InterferenceGraph::Graph &IG,
                                    std::unordered_map<InterferenceGraph::IGNode *, VirtualRegister> &CurrentColoring,
                                    InterferenceGraph::IGNode *NodeToColor) {
  std::set<VirtualRegister> UsedColors;
  // Figure out what colors are used by neighbors
  for (auto Neighbor : IG.at(NodeToColor)) {
    UsedColors.insert(CurrentColoring[Neighbor]);
  }

  VirtualRegister ColorToUse = 0;
  // Find first unused color
  for (auto Reg = 1; Reg <= std::numeric_limits<VirtualRegister>::max(); Reg++) {
    if (UsedColors.find(Reg) == UsedColors.end()) {
      ColorToUse = Reg;
      break;
    }
  }

  if (ColorToUse == 0) {
    throw logic_error("Not enough virtual registers to color interference graph.");
  }

  CurrentColoring[NodeToColor] = ColorToUse;
}

std::string AnnotatedIG::lookupColor(Value *RAV) {
  std::array<std::string, RegisterAllocator::NUM_REGISTERS> const Colors = {
      "lightblue", "lightcyan", "lightgreen", "lightmagenta", "lightred", "lightyellow", "pink", "yellowgreen"};
  if (C->find(RAV) == C->end()) {
    // Value didn't get colored
    return "white";
  }

  // Lookup color in const array
  RegisterAllocator::VirtualRegister Color = C->at(RAV);
  if (Color <= RegisterAllocator::NUM_REGISTERS && Color >= 1) {
    return Colors.at(Color - 1);
  }

  // Spilled register
  return "darkgrey";
}