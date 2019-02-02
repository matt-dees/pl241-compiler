#include "Variable.h"
#include <numeric>

using namespace cs241c;
using namespace std;

Variable::Variable(string Ident, int WordCount) : Ident(move(Ident)), WordCount(WordCount) {}

bool cs241c::Variable::isSingleWord() const { return WordCount == 1; }

string Variable::ident() { return Ident; }

string Variable::toString() const {
  auto Prefix = !isMoveable() ? "&" : "";
  return Prefix + Ident;
}

static int calculateArrayWordSize(const vector<int> &Dim) {
  return accumulate(Dim.begin(), Dim.end(), 1, multiplies<>());
}

GlobalVariable::GlobalVariable(string Ident) : Variable(move(Ident), 1) {}

GlobalVariable::GlobalVariable(string Ident, const vector<int> &ArrayDimensions)
    : Variable(move(Ident), calculateArrayWordSize(ArrayDimensions)) {}

bool GlobalVariable::isMoveable() const { return false; }

LocalVariable::LocalVariable(string Name) : Variable(move(Name), 1), IsArray(false) {}

LocalVariable::LocalVariable(string Name, const vector<int> &ArrayDimensions)
    : Variable(move(Name), calculateArrayWordSize(ArrayDimensions)), IsArray(true) {}

bool LocalVariable::isMoveable() const { return !IsArray; }
