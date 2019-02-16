#ifndef CS241C_IR_VARIABLE_H
#define CS241C_IR_VARIABLE_H

#include "Value.h"
#include <string>
#include <vector>

namespace cs241c {
class Variable : public Value {
protected:
  std::string Ident;
  int WordCount;

  Variable(std::string Ident, int WordCount);

public:
  std::string ident();
  int wordCount() const;
  virtual bool isMoveable() const = 0;
  bool isSingleWord() const;
  std::string toString() const override;
};

class GlobalVariable : public Variable {
public:
  GlobalVariable(std::string Ident);
  GlobalVariable(std::string Ident, const std::vector<int> &ArrayDimensions);

  bool isMoveable() const override;
};

class LocalVariable : public Variable {
  bool IsArray;

public:
  LocalVariable(std::string Name);
  LocalVariable(std::string Name, const std::vector<int> &ArrayDimensions);

  bool isMoveable() const override;
};
} // namespace cs241c

#endif
