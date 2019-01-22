#include "Variable.h"
#include <numeric>

using namespace cs241c;

GlobalVariable::GlobalVariable(std::string Ident)
    : Ident(move(Ident)), WordCount(1) {}

GlobalVariable::GlobalVariable(std::string Ident,
                               const std::vector<int> &ArrayDimensions)
    : Ident(move(Ident)),
      WordCount(std::accumulate(ArrayDimensions.begin(), ArrayDimensions.end(),
                                1, std::multiplies<>())) {}

std::string GlobalVariable::name() { return Ident; }
std::string GlobalVariable::toString() const { return Ident; }

LocalVariable::LocalVariable(std::string Name) : Name(move(Name)) {}

LocalVariable::LocalVariable(std::string Name,
                             const std::vector<int> &ArrayDimensions)
    : Name(move(Name)),
      WordCount(std::accumulate(ArrayDimensions.begin(), ArrayDimensions.end(),
                                1, std::multiplies<>())) {}

std::string LocalVariable::name() { return Name; }
std::string LocalVariable::toString() const { return Name; }
