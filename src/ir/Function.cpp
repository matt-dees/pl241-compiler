#include "Function.h"
#include <unordered_set>

using namespace cs241c;
using namespace std;

Function::Function(string Name, vector<unique_ptr<LocalVariable>> &&Locals) : Name(move(Name)), Locals(move(Locals)) {}

vector<unique_ptr<ConstantValue>> &Function::constants() { return Constants; }

vector<unique_ptr<LocalVariable>> &Function::locals() { return Locals; }

BasicBlock *Function::entryBlock() const { return BasicBlocks.front().get(); }

vector<unique_ptr<BasicBlock>> &Function::basicBlocks() { return BasicBlocks; }
const vector<unique_ptr<BasicBlock>> &Function::basicBlocks() const { return BasicBlocks; }

string Function::toString() const { return Name; }
