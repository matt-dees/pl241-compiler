#include "Value.h"

using namespace cs241c;

Value::Value(const std::string &ID) : ID(ID) {}

const std::string Value::toString() { return ID; }

ConstantValue::ConstantValue(int Val) : Value(std::to_string(Val)), Val(Val) {}
