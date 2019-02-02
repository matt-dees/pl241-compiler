#include "Function.h"
#include <unordered_set>

using namespace cs241c;
using namespace std;

Function::Function(string Name, vector<unique_ptr<ConstantValue>> &&Constants,
                   vector<unique_ptr<LocalVariable>> &&Locals, vector<unique_ptr<BasicBlock>> &&BasicBlocks)
    : Name(move(Name)), Constants(move(Constants)), Locals(move(Locals)), BasicBlocks(move(BasicBlocks)) {}

vector<unique_ptr<ConstantValue>> &Function::constants() { return Constants; }

vector<unique_ptr<LocalVariable>> &Function::locals() { return Locals; }

BasicBlock *Function::entryBlock() const { return BasicBlocks.front().get(); }

const vector<unique_ptr<BasicBlock>> &Function::basicBlocks() const { return BasicBlocks; }

string Function::toString() const { return Name; }
