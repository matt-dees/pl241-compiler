#include "InterferenceGraph.h"

using namespace cs241c;
using namespace std;

void InterferenceGraph::addInterferences(const std::unordered_set<Value *> &FromSet, Value *To) {
  for (auto Node : FromSet) {
    addInterference(Node, To);
  }
}

void InterferenceGraph::addInterference(Value *From, Value *To) {
  if (From == To) {
    return;
  }
  auto IGFrom = ValueToNode.at(From);
  auto IGTo = ValueToNode.at(To);
  IG[IGFrom].insert(IGTo);
  IG[IGTo].insert(IGFrom);
}

std::unordered_set<InterferenceGraph::IGNode *> InterferenceGraph::neighbors(IGNode *Node) {
  if (IG.find(Node) == IG.end()) {
    throw logic_error("Node not in graph.");
  }
  return IG.at(Node);
}

void InterferenceGraph::removeNode(IGNode *Node) {
  if (IG.find(Node) == IG.end()) {
    return;
  }
  for (auto Neighbor : IG.at(Node)) {
    IG.at(Neighbor).erase(Node);
  }
  IG.erase(Node);
}

bool InterferenceGraph::hasValue(Value *Node) { return ValueToNode.find(Node) != ValueToNode.end(); }

void InterferenceGraph::addValue(Value *Val) {
  if (hasValue(Val)) {
    return;
  }
  auto Node = std::make_unique<struct IGNode>(Val);
  ValueToNode[Val] = Node.get();
  IG[Node.get()] = {};
  IGNodes.push_back(move(Node));
}

void InterferenceGraph::coalesce() {
  for (auto &Node : IGNodes) {
    IGNode *CurrentNode = Node.get();
    if (!hasNode(CurrentNode)) {
      continue;
    }
    for (auto Val : CurrentNode->Values) {
      if (auto Instr = dynamic_cast<Instruction *>(Val)) {
        if (Instr->InstrT == InstructionType::Phi) {
          auto PhiArgs = Instr->arguments();
          auto LeftArg = ValueToNode[PhiArgs[0]];
          auto RightArg = ValueToNode[PhiArgs[1]];
          if (hasNode(LeftArg) && hasNode(RightArg) && !interferes(CurrentNode, LeftArg) &&
              !interferes(CurrentNode, RightArg) && !interferes(LeftArg, RightArg)) {
            for (auto NodeArg : std::unordered_set<IGNode *>{LeftArg, RightArg}) {
              CurrentNode->merge(NodeArg);
              removeNode(NodeArg);
            }
            ValueToNode[PhiArgs[0]] = CurrentNode;
            ValueToNode[PhiArgs[1]] = CurrentNode;
          }
        }
      }
    }
  }
}

bool InterferenceGraph::interferes(IGNode *Node1, IGNode *Node2) {
  auto Node2Neighbors = neighbors(Node2);
  return Node2Neighbors.find(Node1) != Node2Neighbors.end();
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
    CurrentCtx.SHC.LoopDepth++;
    IgBuildCtx ReturnCtx = igBuildLoop(CurrentCtx);
    ReturnCtx.SHC.LoopDepth--;
    return ReturnCtx;
  } else if (CurrentCtx.NextNode->hasAttribute(BasicBlockAttr::Join)) {
    return igBuildIfStmt(CurrentCtx);
  }
  return igBuildNormal(CurrentCtx);
}

IGBuilder::IgBuildCtx IGBuilder::igBuildNormal(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorSets = processBlock(CurrentCtx);

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
IGBuilder::processBlock(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorLiveSets;
  for (auto Val : CurrentCtx.LiveSet) {
    IG.addInterferences(CurrentCtx.LiveSet, Val);
  }
  for (auto ReverseInstructionIt = CurrentCtx.NextNode->instructions().rbegin();
       ReverseInstructionIt != CurrentCtx.NextNode->instructions().rend(); ReverseInstructionIt++) {

    CurrentCtx.LiveSet.erase(ReverseInstructionIt->get());
    auto Args = ReverseInstructionIt->get()->arguments();
    for (auto i = 0; i < Args.size(); i++) {
      auto Arg = Args.at(i);
      if (Arg.ValTy == ValueType::StackSlot || Arg.ValTy == ValueType::Parameter ||
          dynamic_cast<Instruction *>(Arg.Ptr) == nullptr ||
          dynamic_cast<Instruction *>(Arg.Ptr)->InstrT == InstructionType::Adda) {
        continue;
      }
      IG.addValue(Arg);
      IG.visit(Arg, CurrentCtx.SHC);
      IG.addInterferences(CurrentCtx.LiveSet, Arg);
      if (ReverseInstructionIt->get()->InstrT != InstructionType::Phi) {
        CurrentCtx.LiveSet.insert(Arg);
      } else {
        auto Pred = CurrentCtx.NextNode->predecessors().at(i);
        if (PredecessorLiveSets.find(Pred) == PredecessorLiveSets.end()) {
          PredecessorLiveSets[Pred] = {Arg};
        } else {
          PredecessorLiveSets[Pred].insert(Arg);
        }
      }
    }
  }

  for (auto Pred : CurrentCtx.NextNode->predecessors()) {
    std::unordered_set<Value *> LiveSetToPropagate = {};
    if (PredecessorLiveSets.find(Pred) != PredecessorLiveSets.end()) {
      LiveSetToPropagate = PredecessorLiveSets[Pred];
    }
    copy(CurrentCtx.LiveSet.begin(), CurrentCtx.LiveSet.end(), inserter(LiveSetToPropagate, LiveSetToPropagate.end()));
    PredecessorLiveSets[Pred] = LiveSetToPropagate;
  }
  return PredecessorLiveSets;
}

void InterferenceGraph::visit(Value *Val, const SpillHeuristicCtx &SHC) {
  IGNode *Node = ValueToNode.at(Val);
  Node->CurrentSpillCost += 1 + SHC.LoopDepth * SHC.LOOP_MULTIPLIER;
}

IGBuilder::IgBuildCtx IGBuilder::igBuildIfStmt(IGBuilder::IgBuildCtx CurrentCtx) {
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorPhiSets = processBlock(CurrentCtx);

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
  std::unordered_map<BasicBlock *, std::unordered_set<Value *>> PredecessorPhiSets = processBlock(CurrentCtx);
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
  LoopContext = {CurrentCtx.NextNode, LoopContext.LiveSet};
  PredecessorPhiSets = processBlock(LoopContext);
  return {ContinuePredecessor, PredecessorPhiSets.at(ContinuePredecessor)};
}

void IGBuilder::IgBuildCtx::merge(IGBuilder::IgBuildCtx Other) {
  if (NextNode != nullptr && Other.NextNode != NextNode) {
    throw logic_error("Context merge failure in IGBuilder.");
  }
  LiveSet.insert(Other.LiveSet.begin(), Other.LiveSet.end());
  NextNode = Other.NextNode;
}
