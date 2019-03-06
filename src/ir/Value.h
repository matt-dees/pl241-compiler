#ifndef CS241C_IR_VALUE_H
#define CS241C_IR_VALUE_H

#include <string>
#include <string_view>

namespace cs241c {
enum class ValueType {
  Undef,
  Any,
  Unit,
  Value,
  Constant,
  Variable,
  Register,
  Parameter,
  StackSlot,
  BasicBlock,
  Function
};

bool isSubtype(ValueType This, ValueType Super);
std::string_view name(ValueType);

class Value {
public:
  const ValueType ValTy;

protected:
  Value(ValueType);

public:
  virtual ~Value() = default;
  virtual std::string name() const;
  virtual std::string toString() const = 0;
};

class ConstantValue : public Value {
public:
  int Val;

  ConstantValue(int Val);
  std::string toString() const override;
};

class NamedValue : public Value {
  std::string Name;

public:
  NamedValue(std::string Name);
  std::string toString() const override;
};
} // namespace cs241c

#endif
