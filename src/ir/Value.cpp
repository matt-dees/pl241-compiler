#include "Value.h"
#include <type_traits>

using namespace cs241c;
using namespace std;

using ValTyT = ValueType;

struct ValTyInfo {
  const ValTyT SuperType;
  const string_view Name;
};

static ValTyInfo SuperTypes[] = {{ValTyT::Undef, "Undef"},       {ValTyT::Any, "Any"},
                                 {ValTyT::Any, "Unit"},          {ValTyT::Any, "Value"},
                                 {ValTyT::Value, "Constant"},    {ValTyT::Value, "Variable"},
                                 {ValTyT::Variable, "Register"}, {ValTyT::Variable, "StackSlot"},
                                 {ValTyT::Value, "Cmp"},         {ValTyT::Value, "Adda"},
                                 {ValTyT::Any, "BasicBlock"},    {ValTyT::Any, "Function"}};

bool cs241c::isSubtype(ValTyT This, ValTyT Super) {
  if (This == ValTyT::Undef) {
    return This == Super;
  }

  while (This != ValTyT::Any) {
    if (This == Super)
      return true;
    auto IdThis = static_cast<underlying_type<ValTyT>::type>(This);
    This = SuperTypes[IdThis].SuperType;
  }
  return This == Super;
}

std::string_view cs241c::name(ValueType ValTy) {
  auto IdThis = static_cast<underlying_type<ValTyT>::type>(ValTy);
  return SuperTypes[IdThis].Name;
}

Value::Value(ValueType ValTy) : ValTy(ValTy) {}

string Value::name() const { return toString(); }

ConstantValue::ConstantValue(int Val) : Value(ValueType::Constant), Val(Val) {}
string ConstantValue::toString() const { return string("#") + to_string(Val); }

NamedValue::NamedValue(string Name) : Value(ValueType::Value), Name(move(Name)) {}
string NamedValue::toString() const { return Name; }
