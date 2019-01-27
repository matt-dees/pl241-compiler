#include "Variable.h"
#include <numeric>

using namespace cs241c;

Variable::Variable(std::string Ident, int WordCount)
    : Ident(move(Ident)), WordCount(WordCount) {}

std::string Variable::name() { return Ident; }

std::string Variable::toString() const {
  auto Prefix = !isMoveable() ? "&" : "";
  return Prefix + Ident;
}

GlobalVariable::GlobalVariable(std::string Ident) : Variable(move(Ident), 4) {}

GlobalVariable::GlobalVariable(std::string Ident,
                               const std::vector<int> &ArrayDimensions)
    : Variable(move(Ident),
               std::accumulate(ArrayDimensions.begin(), ArrayDimensions.end(),
                               4, std::multiplies<>())) {}

bool GlobalVariable::isMoveable() const { return false; }

void GlobalVariable::visit(VariableVisitor *V) { V->visit(this); }

LocalVariable::LocalVariable(std::string Name)
    : Variable(move(Name), 4), IsArray(false) {}

LocalVariable::LocalVariable(std::string Name,
                             const std::vector<int> &ArrayDimensions)
    : Variable(move(Name),
               std::accumulate(ArrayDimensions.begin(), ArrayDimensions.end(),
                               4, std::multiplies<>())),
      IsArray(true) {}

bool LocalVariable::isMoveable() const { return !IsArray; }

void LocalVariable::visit(VariableVisitor *V) { V->visit(this); }
