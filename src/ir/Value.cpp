#include "Value.h"

using namespace cs241c;
using namespace std;

string Value::name() const { return toString(); }

ConstantValue::ConstantValue(int Val) : Val(Val) {}
string ConstantValue::toString() const { return string("#") + to_string(Val); }

NamedValue::NamedValue(string Name) : Name(move(Name)) {}
string NamedValue::toString() const { return Name; }
