#include "GlobalVariable.h"
#include <numeric>

using namespace cs241c;

GlobalVariable::GlobalVariable(std::string Ident)
    : Value(move(Ident)), WordCount(1) {}

GlobalVariable::GlobalVariable(std::string Ident,
                               const std::vector<int> &ArrayDimensions)
    : Value(move(Ident)),
      WordCount(std::accumulate(ArrayDimensions.begin(), ArrayDimensions.end(),
                                1, std::multiplies<int>())) {}
