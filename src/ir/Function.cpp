#include "Function.h"
#include "RegisterAllocator.h"
#include <algorithm>
#include <iterator>
#include <stack>
#include <unordered_set>

using namespace cs241c;
using namespace std;

Function::Function(string Name, vector<unique_ptr<LocalVariable>> &&Locals)
    : Value(ValueType::Function), Name(move(Name)), Locals(move(Locals)) {}

vector<unique_ptr<ConstantValue>> &Function::constants() { return Constants; }

vector<unique_ptr<LocalVariable>> &Function::locals() { return Locals; }

BasicBlock *Function::entryBlock() const { return BasicBlocks.front().get(); }

vector<unique_ptr<BasicBlock>> &Function::basicBlocks() { return BasicBlocks; }
const vector<unique_ptr<BasicBlock>> &Function::basicBlocks() const {
  return BasicBlocks;
}

string Function::toString() const { return Name; }

std::vector<BasicBlock *> Function::postOrderCfg() {
  stack<BasicBlock *> WorkStack;
  unordered_set<BasicBlock *> Visited;
  std::vector<BasicBlock *> PostOrdering;

  WorkStack.push(entryBlock());

  while (!WorkStack.empty()) {
    BasicBlock *B = WorkStack.top();
    Visited.insert(B);

    bool HasUnvisitedFollower = false;
    for (BasicBlock *Follower : B->successors()) {
      if (Visited.find(Follower) == Visited.end()) {
        HasUnvisitedFollower = true;
        WorkStack.push(Follower);
      }
    }

    if (HasUnvisitedFollower)
      continue;

    WorkStack.pop();
    PostOrdering.push_back(B);
  }
  return PostOrdering;
}