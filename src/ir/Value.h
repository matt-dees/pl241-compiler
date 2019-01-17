#ifndef CS241C_IR_VALUE_H
#define CS241C_IR_VALUE_H

#include <string>

namespace cs241c {
class Value {
public:
  virtual ~Value() = default;
  virtual std::string id() const;
  virtual std::string toString() const = 0;
};

class ConstantValue : public Value {
public:
  int Val;

  explicit ConstantValue(int Val);
  std::string toString() const override;
};
} // namespace cs241c

#endif
