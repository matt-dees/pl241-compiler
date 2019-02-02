#include "Variable.h"
#include <numeric>

using namespace cs241c;

Variable::Variable(std::string Ident, int WordCount)
    : Ident(move(Ident)), WordCount(WordCount) {}

bool cs241c::Variable::isSingleWord() const { return WordCount == 1; }

std::string Variable::ident() { return Ident; }

std::string Variable::toString() const {
  auto Prefix = !isMoveable() ? "&" : "";
  return Prefix + Ident;
}

static int calculateArrayWordSize(const std::vector<int> &Dim) {
  return std::accumulate(Dim.begin(), Dim.end(), 1, std::multiplies<>());
}

GlobalVariable::GlobalVariable(std::string Ident) : Variable(move(Ident), 1) {}

GlobalVariable::GlobalVariable(std::string Ident,
                               const std::vector<int> &ArrayDimensions)
    : Variable(move(Ident), calculateArrayWordSize(ArrayDimensions)) {}

bool GlobalVariable::isMoveable() const { return false; }

LocalVariable::LocalVariable(std::string Name)
    : Variable(move(Name), 1), IsArray(false) {}

LocalVariable::LocalVariable(std::string Name,
                             const std::vector<int> &ArrayDimensions)
    : Variable(move(Name), calculateArrayWordSize(ArrayDimensions)),
      IsArray(true) {}

bool LocalVariable::isMoveable() const { return !IsArray; }
