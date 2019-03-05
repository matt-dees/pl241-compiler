#include "RegisterAllocator.h"
#include <iostream>
#include <limits>
#include <set>
#include <stack>
#include <utility>

using namespace cs241c;
using namespace std;

RegisterAllocator::Coloring RegisterAllocator::color(InterferenceGraph IG) {
  if (IG.graph().size() == 0) {
    return {};
  }

  Coloring CurrentColoring = {};
  color(IG, CurrentColoring);
  for (auto Node : IG.coalescedNodes()) {
    CurrentColoring[Node.first] = CurrentColoring.at(Node.second);
  }
  return CurrentColoring;
}

static void addNodeBack(InterferenceGraph &IG, Value *Node, std::unordered_set<Value *> &Neighbors) {
  IG.addNode(Node);
  for (auto Neighbor : Neighbors) {
    if (IG.hasNode(Neighbor)) {
      IG.addEdge(Node, Neighbor);
    }
  }
}

void RegisterAllocator::color(InterferenceGraph &IG, RegisterAllocator::Coloring &CurrentColoring) {
  stack<pair<Value *, unordered_set<Value *>>> RemovedNodes;

  while (IG.graph().size() > 1) {
    Value *NextNode = chooseNextNodeToColor(IG);
    std::unordered_set<Value *> NeighborsOfRemoved = IG.removeNode(NextNode);
    RemovedNodes.push({NextNode, move(NeighborsOfRemoved)});
  }

  assignColor(IG, CurrentColoring, IG.graph().begin()->first);

  while (!RemovedNodes.empty()) {
    auto &NextNode = RemovedNodes.top();
    addNodeBack(IG, NextNode.first, NextNode.second);
    assignColor(IG, CurrentColoring, NextNode.first);
    RemovedNodes.pop();
  }
}

Value *RegisterAllocator::getValWithLowestSpillCost(InterferenceGraph &IG) {
  // TODO: Change implementation of this function
  // to use priority queue data structure.
  Value *ToReturn = nullptr;
  int32_t CurrentMinSpillCost = std::numeric_limits<int32_t>::max();
  for (auto RAVPair : IG.graph()) {
    /*if (auto Instr = dynamic_cast<Instruction *>(RAVPair.first)) {
      if (Instr->DontSpill) {
        continue;
      }
    }*/

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

void RegisterAllocator::assignColor(InterferenceGraph &IG, RegisterAllocator::Coloring &CurrentColoring,
                                    Value *NodeToColor) {
  std::set<VirtualRegister> UsedColors;
  // Figure out what colors are used by neighbors
  for (auto Neighbor : IG.neighbors(NodeToColor)) {
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
