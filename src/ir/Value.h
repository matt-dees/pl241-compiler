#ifndef CS241C_IR_VALUE_H
#define CS241C_IR_VALUE_H

namespace cs241c {
class Value {};

class ConstantValue : public Value {
public:
  int Val;

  explicit ConstantValue(int Val);
};
} // namespace cs241c

#endif
