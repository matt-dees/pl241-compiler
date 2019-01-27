#include "Value.h"

using namespace cs241c;

std::string Value::id() const { return toString(); }

ConstantValue::ConstantValue(int Val) : Val(Val) {}
std::string ConstantValue::toString() const { return std::to_string(Val); }

NamedValue::NamedValue(std::string Name) : Name(move(Name)) {}
std::string NamedValue::toString() const { return Name; }
