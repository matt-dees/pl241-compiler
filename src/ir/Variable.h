#ifndef CS241C_IR_VARIABLE_H
#define CS241C_IR_VARIABLE_H

#include "Value.h"
#include <string>
#include <vector>

namespace cs241c {
class Variable : public Value {};

class GlobalVariable : public Variable {
public:
  std::string Ident;
  int WordCount;

  GlobalVariable(std::string Ident);
  GlobalVariable(std::string Ident, const std::vector<int> &ArrayDimensions);

  std::string name();
  std::string toString() const override;
};

class LocalVariable : public Variable {
  std::string Name;
  int WordCount;

public:
  LocalVariable(std::string Name);
  LocalVariable(std::string Name, const std::vector<int> &ArrayDimensions);

  std::string name();
  std::string toString() const override;
};
} // namespace cs241c

#endif
