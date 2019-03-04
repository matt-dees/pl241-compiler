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

void InterferenceGraph::addEdges(const std::unordered_set<Value *> &FromSet, Value *To) {
  for (auto Node : FromSet) {
    addEdge(Node, To);
  }
}

void InterferenceGraph::addEdge(Value *From, Value *To) {
  if (From == To) {
    return;
  }
  IG[From].insert(To);
  IG[To].insert(From);
}

void InterferenceGraph::writeNodes(ofstream &OutStream) {
  for (auto &VertexEdgePair : IG) {
    OutStream << "node: {\n";
    OutStream << "title: "
              << "\"" << VertexEdgePair.first->toString() << "\"\n";
    OutStream << "label: \"" + VertexEdgePair.first->toString() << "\"\n";

    OutStream << "label: \"" + VertexEdgePair.first->toString() << "\n";
    for (auto Val : coalescedNodes()) {
      if (Val.second == VertexEdgePair.first) {
        OutStream << Val.first->toString() << "\n";
      }
    }
    OutStream << "\"";
    OutStream << "color:white"
              << "\n";
    OutStream << "}\n";
    for (auto Destination : VertexEdgePair.second) {
      writeEdge(OutStream, VertexEdgePair.first, Destination);
    }
  }
}

void InterferenceGraph::writeEdge(ofstream &OutStream, Value *From, Value *To) {
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

std::unordered_set<Value *> InterferenceGraph::neighbors(Value *RAV) {
  if (IG.find(RAV) == IG.end()) {
    throw logic_error("Value " + RAV->toString() + " not in graph.");
  }
  return IG.at(RAV);
}

std::unordered_set<Value *> InterferenceGraph::removeNode(Value *RAV) {
  std::unordered_set<Value *> Neighbors = neighbors(RAV);
  IG.erase(RAV);
  for (auto Neighbor : Neighbors) {
    IG[Neighbor].erase(RAV);
  }
  return Neighbors;
}

bool InterferenceGraph::hasNode(Value *Node) { return IG.find(Node) != IG.end(); }

void InterferenceGraph::addNode(Value *Node) {
  if (hasNode(Node)) {
    return;
  }
  IG[Node] = {};
}

RAHeuristicInfo &InterferenceGraph::heuristicData(Value *Val) {
  if (HeuristicDataMap.find(Val) == HeuristicDataMap.end()) {
    HeuristicDataMap[Val] = RAHeuristicInfo();
  }
  return HeuristicDataMap[Val];
}

void InterferenceGraph::coalesce() {
  for (auto NodeEdgePair : graph()) {
    if (auto Instr = dynamic_cast<Instruction *>(NodeEdgePair.first)) {
      auto GraphInstr = getValueInGraph(Instr);
      if (Instr->InstrT == InstructionType::Phi) {
        auto PhiArgs = Instr->arguments();
        auto LeftArg = getValueInGraph(PhiArgs.at(0));
        auto RightArg = getValueInGraph(PhiArgs.at(1));
        if (hasNode(LeftArg) && hasNode(RightArg) && !interferes(GraphInstr, LeftArg) &&
            !interferes(GraphInstr, RightArg) && !interferes(LeftArg, RightArg)) {
          for (auto Node : std::unordered_set<Value *>{LeftArg, RightArg}) {
            addEdges(neighbors(Node), GraphInstr);
            removeNode(Node);
            CoalesceMap[Node] = GraphInstr;
            for (auto CoalesceMapping : CoalesceMap) {
              if (CoalesceMapping.second == Node) {
                CoalesceMap[CoalesceMapping.first] = GraphInstr;
              }
            }
          }
        }
      }
    }
  }
}

bool InterferenceGraph::interferes(Value *Node1, Value *Node2) {
  std::unordered_set<Value *> Node2Neighbors = neighbors(Node2);
  return Node2Neighbors.find(Node1) != Node2Neighbors.end();
}

Value *InterferenceGraph::getValueInGraph(Value *V) {
  if (CoalesceMap.find(V) == CoalesceMap.end()) {
    return V;
  }
  return CoalesceMap[V];
}

void IGBuilder::buildInterferenceGraph() {
  for (auto ExitBB : F->exitBlocks()) {
    IgBuildCtx Ctx{ExitBB, {}};
    while (Ctx.NextNode != nullptr) {
      Ctx = igBuild(Ctx);
    }
  }
  IG.coalesce();
}

IGBuilder::IgBuildCtx IGBuilder::igBuild(IGBuilder::IgBuildCtx CurrentCtx) {
  if (CurrentCtx.NextNode->hasAttribute(BasicBlockAttr::While)) {
    return igBuildLoop(CurrentCtx);
  } else if (CurrentCtx.NextNode->hasAttribute(BasicBlockAttr::Join)) {
    return igBuildIfStmt(CurrentCtx);
  }
  return igBuildNormal(CurrentCtx);
}

IGBuilder::IgBuildCtx IGBuilder::igBuildNormal(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorSets =
      processBlock(CurrentCtx.NextNode, CurrentCtx.LiveSet);

  IgBuildCtx NextCtx;
  if (PredecessorSets.size() > 1) {
    throw logic_error("igBuildNormal() incorrectly called on basic block with "
                      "more than one predecessor: " +
                      CurrentCtx.NextNode->toString());
  }
  if (PredecessorSets.size() == 1) {
    NextCtx = {PredecessorSets.begin()->first, PredecessorSets.begin()->second};
  }
  return NextCtx;
}

std::unordered_map<BasicBlock *, std::unordered_set<Value *>>
IGBuilder::processBlock(BasicBlock *BB, std::unordered_set<Value *> LiveSet) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorLiveSets;
  for (auto Val : LiveSet) {
    IG.addEdges(LiveSet, Val);
  }
  for (auto ReverseInstructionIt = BB->instructions().rbegin(); ReverseInstructionIt != BB->instructions().rend();
       ReverseInstructionIt++) {

    LiveSet.erase(ReverseInstructionIt->get());
    auto Args = ReverseInstructionIt->get()->arguments();
    for (auto i = 0; i < Args.size(); i++) {
      auto Arg = Args.at(i);
      IG.heuristicData(Arg).NumUses++;
      if (dynamic_cast<Instruction *>((Value *)Arg) == nullptr) {
        continue;
      }
      IG.addNode(Arg);
      IG.addEdges(LiveSet, Arg);
      if (ReverseInstructionIt->get()->InstrT != InstructionType::Phi) {
        LiveSet.insert(Arg);
      } else {
        auto Pred = BB->predecessors().at(i);
        if (PredecessorLiveSets.find(Pred) == PredecessorLiveSets.end()) {
          PredecessorLiveSets[Pred] = {Arg};
        } else {
          PredecessorLiveSets[Pred].insert(Arg);
        }
      }
    }
  }

  for (auto Pred : BB->predecessors()) {
    std::unordered_set<Value *> LiveSetToPropagate = {};
    if (PredecessorLiveSets.find(Pred) != PredecessorLiveSets.end()) {
      LiveSetToPropagate = PredecessorLiveSets[Pred];
    }
    copy(LiveSet.begin(), LiveSet.end(), inserter(LiveSetToPropagate, LiveSetToPropagate.end()));
    PredecessorLiveSets[Pred] = LiveSetToPropagate;
  }
  return PredecessorLiveSets;
}

IGBuilder::IgBuildCtx IGBuilder::igBuildIfStmt(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorPhiSets =
      processBlock(CurrentCtx.NextNode, CurrentCtx.LiveSet);

  IgBuildCtx IfHeaderCtx;
  for (auto Pred : CurrentCtx.NextNode->predecessors()) {
    IgBuildCtx BranchCtx = {Pred, PredecessorPhiSets[Pred]};
    while (!BranchCtx.NextNode->hasAttribute(BasicBlockAttr::If)) {
      BranchCtx = igBuild(BranchCtx);
    }
    IfHeaderCtx.merge(BranchCtx);
  }
  return igBuild(IfHeaderCtx);
}

IGBuilder::IgBuildCtx IGBuilder::igBuildLoop(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorPhiSets =
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
    throw logic_error("Tried to build interference graph for loop block " + CurrentCtx.NextNode->toString() +
                      " but it does not have a loop predecessor.");
  }

  bool InSecondIteration = false;
  IgBuildCtx LoopContext = {LoopPredecessor, PredecessorPhiSets.at(LoopPredecessor)};
  while (LoopContext.NextNode != CurrentCtx.NextNode || !InSecondIteration) {
    if (LoopContext.NextNode == CurrentCtx.NextNode) {
      InSecondIteration = true;
      LoopContext = {LoopPredecessor, LoopContext.LiveSet};
    }
    LoopContext = igBuild(LoopContext);
  }
  PredecessorPhiSets = processBlock(CurrentCtx.NextNode, LoopContext.LiveSet);
  return {ContinuePredecessor, PredecessorPhiSets.at(ContinuePredecessor)};
}

void IGBuilder::IgBuildCtx::merge(IGBuilder::IgBuildCtx Other) {
  if (NextNode != nullptr && Other.NextNode != NextNode) {
    throw logic_error("Context merge failure in IGBuilder.");
  }
  LiveSet.insert(Other.LiveSet.begin(), Other.LiveSet.end());
  NextNode = Other.NextNode;
}
