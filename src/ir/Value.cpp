#include "Value.h"

using namespace cs241c;

ConstantValue::ConstantValue(int Val) : Val(Val) {}

std::string ConstantValue::toString() const { return std::to_string(Val); }
