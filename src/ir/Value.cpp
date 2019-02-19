#include "Value.h"

using namespace cs241c;
using namespace std;

Value::Value(ValueType ValTy) : ValTy(ValTy) {}

string Value::name() const { return toString(); }

ConstantValue::ConstantValue(int Val) : Value(ValueType::Constant), Val(Val) {}
string ConstantValue::toString() const { return string("#") + to_string(Val); }

NamedValue::NamedValue(string Name) : Value(ValueType::Value), Name(move(Name)) {}
string NamedValue::toString() const { return Name; }
