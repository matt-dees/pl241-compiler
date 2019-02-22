#include "InterferenceGraph.h"

using namespace cs241c;
using namespace std;

void InterferenceGraph::writeGraph(ofstream &OutStream) {
  reset();
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

void IGBuilder::buildInterferenceGraph() {
  std::unordered_set<RegAllocValue *> LiveSet = {};
  BasicBlock *Start = F->entryBlock();
  igBuild({Start, LiveSet});
}

RegAllocValue *IGBuilder::lookupRegAllocVal(Value *Val) {
  if (ValueToRegAllocVal.find(Val) == ValueToRegAllocVal.end()) {
    ValueToRegAllocVal[Val] = std::make_unique<RegAllocValue>(Val);
  }

  return ValueToRegAllocVal[Val].get();
}

IGBuilder::IgBuildCtx IGBuilder::igBuild(IGBuilder::IgBuildCtx CurrentCtx) {
  if (DT->isJoinBlock(CurrentCtx.NextNode)) {
    return igBuildIfStmt(CurrentCtx);
  } else if (DT->isLoopHdrBlock(CurrentCtx.NextNode)) {
    return igBuildLoop(CurrentCtx);
  }
  return igBuildNormal(CurrentCtx);
}

IGBuilder::IgBuildCtx
IGBuilder::igBuildNormal(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<RegAllocValue *>>
      PredecessorSets = processBlock(CurrentCtx.NextNode, CurrentCtx.LiveSet);
  return {PredecessorSets.empty() ? nullptr : PredecessorSets.begin()->first,
          PredecessorSets.begin()->second};
}

std::unordered_map<BasicBlock *, std::unordered_set<RegAllocValue *>>
IGBuilder::processBlock(BasicBlock *BB,
                        std::unordered_set<RegAllocValue *> LiveSet) {
  std::unordered_map<BasicBlock *, std::unordered_set<RegAllocValue *>>
      PredecessorPhiSets;
  for (auto ReverseInstructionIt = BB->instructions().rbegin();
       ReverseInstructionIt != BB->instructions().rend();
       ReverseInstructionIt++) {

    LiveSet.erase(lookupRegAllocVal(ReverseInstructionIt->get()));
    for (auto Arg : ReverseInstructionIt->get()->arguments()) {
      RegAllocValue *RegAlArg = lookupRegAllocVal(Arg);
      RegAlArg->visit();
      if (dynamic_cast<Instruction *>(RegAlArg->value()) == nullptr) {
        continue;
      }
      IG.addEdges(LiveSet, RegAlArg);
      if (ReverseInstructionIt->get()->InstrT == InstructionType::Phi) {
        for (auto Pred : BB->predecessors()) {
          if (PredecessorPhiSets.find(Pred) != PredecessorPhiSets.end()) {
            IG.addEdges(PredecessorPhiSets[Pred], RegAlArg);
            PredecessorPhiSets[Pred].insert(RegAlArg);
          } else {
            PredecessorPhiSets[Pred] = {RegAlArg};
          }
        }
      } else {
        LiveSet.insert(RegAlArg);
      }
    }
  }
  return PredecessorPhiSets;
}

IGBuilder::IgBuildCtx
IGBuilder::igBuildIfStmt(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<RegAllocValue *>>
      PredecessorPhiSets =
          processBlock(CurrentCtx.NextNode, CurrentCtx.LiveSet);

  IgBuildCtx Ctx;
  for (auto Pred : CurrentCtx.NextNode->predecessors()) {
    std::unordered_set<RegAllocValue *> PhiSetToPropagate = {};
    if (PredecessorPhiSets.find(Pred) != PredecessorPhiSets.end()) {
      PhiSetToPropagate = PredecessorPhiSets[Pred];
    }
    copy(CurrentCtx.LiveSet.begin(), CurrentCtx.LiveSet.end(),
         inserter(PhiSetToPropagate, PhiSetToPropagate.end()));
    Ctx.merge(igBuild({Pred, PhiSetToPropagate}));
  }
  return igBuild(Ctx);
}

IGBuilder::IgBuildCtx IGBuilder::igBuildLoop(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<RegAllocValue *>>
      PredecessorPhiSets =
          processBlock(CurrentCtx.NextNode, CurrentCtx.LiveSet);
  BasicBlock *LoopPredecessor = nullptr;
  BasicBlock *ContinuePredecessor = nullptr;
  for (auto P : CurrentCtx.NextNode->predecessors()) {
    if (DT->doesBlockDominate(CurrentCtx.NextNode, P)) {
      LoopPredecessor = P;
    } else {
      ContinuePredecessor = P;
    }
  }
  if (!LoopPredecessor) {
    throw logic_error("Tried to build interference graph for loop block " +
                      CurrentCtx.NextNode->toString() +
                      " but it does not have a loop predecessor.");
  }

  bool InSecondIteration = false;
  IgBuildCtx LoopContext = {LoopPredecessor,
                            PredecessorPhiSets.at(LoopPredecessor)};
  while (LoopContext.NextNode != CurrentCtx.NextNode && !InSecondIteration) {
    if (LoopContext.NextNode == CurrentCtx.NextNode) {
      InSecondIteration = true;
    }
    LoopContext = igBuild(LoopContext);
  }
  return {ContinuePredecessor, LoopContext.LiveSet};
}

void IGBuilder::IgBuildCtx::merge(IGBuilder::IgBuildCtx Other) {
  if (NextNode != nullptr && Other.NextNode != NextNode) {
    throw logic_error("Context merge failure in IGBuilder.");
  }
  LiveSet.insert(Other.LiveSet.begin(), Other.LiveSet.end());
}