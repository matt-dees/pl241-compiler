#ifndef CS241C_IR_VALUE_H
#define CS241C_IR_VALUE_H

#include <string>

namespace cs241c {
class Value {
protected:
  Value() = default;
  explicit Value(const std::string &ID);

public:
  const std::string toString();

private:
  const std::string ID;
};

class ConstantValue : public Value {
public:
  int Val;

  explicit ConstantValue(int Val);
};
} // namespace cs241c

#endif
