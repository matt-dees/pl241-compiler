#include "RegisterAllocator.h"
#include <iostream>

using namespace cs241c;
using namespace std;

void InterferenceGraph::writeGraph(ofstream &OutStream) {
  WrittenEdges = {};
  OutStream << "title: "
            << "\""
            << "Interference Graph"
            << "\"\n";
  writeNodes(OutStream);
}

void InterferenceGraph::addEdges(const std::unordered_set<Value *> &FromSet,
                                 Value *To) {
  for (auto Node : FromSet) {
    // Bidirectional graph
    addEdge(Node, To);
    addEdge(To, Node);
  }
}

void InterferenceGraph::addEdge(Value *From, Value *To) {
  if (From == To) {
    return;
  }
  if (IG.find(From) == IG.end()) {
    IG[From] = {To};
  } else {
    IG[From].insert(To);
  }
}

void InterferenceGraph::writeNodes(ofstream &OutStream) {
  for (auto &VertexEdgePair : IG) {
    OutStream << "node: {\n";
    OutStream << "title: "
              << "\"" << VertexEdgePair.first->toString() << "\"\n";
    OutStream << "label: \"" + VertexEdgePair.first->toString() << "\"\n";
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
  OutStream << "}\n";
}