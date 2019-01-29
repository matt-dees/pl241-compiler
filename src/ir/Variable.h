#ifndef CS241C_IR_VARIABLE_H
#define CS241C_IR_VARIABLE_H

#include "Value.h"
#include <string>
#include <vector>

namespace cs241c {
class GlobalVariable;
class LocalVariable;

class VariableVisitor {
public:
  virtual ~VariableVisitor() = default;

  virtual void visit(GlobalVariable *) = 0;
  virtual void visit(LocalVariable *) = 0;
};

class Variable : public Value {
protected:
  std::string Ident;
  int WordCount;

  Variable(std::string Ident, int WordCount);

public:
  virtual bool isMoveable() const = 0;
  std::string name();
  std::string toString() const override;

  virtual void visit(VariableVisitor *) = 0;
};

class GlobalVariable : public Variable {
public:
  GlobalVariable(std::string Ident);
  GlobalVariable(std::string Ident, const std::vector<int> &ArrayDimensions);

  bool isMoveable() const override;
  virtual void visit(VariableVisitor *) override;
};

class LocalVariable : public Variable {
  bool IsArray;

public:
  LocalVariable(std::string Name);
  LocalVariable(std::string Name, const std::vector<int> &ArrayDimensions);

  bool isMoveable() const override;
  virtual void visit(VariableVisitor *) override;
};
} // namespace cs241c

#endif
