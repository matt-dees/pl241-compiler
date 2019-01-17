#include "Value.h"

using namespace cs241c;

std::string Value::id() const { return toString(); }

ConstantValue::ConstantValue(int Val) : Val(Val) {}

std::string ConstantValue::toString() const { return std::to_string(Val); }
