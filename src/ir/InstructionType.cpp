#include "InstructionType.h"
#include <algorithm>
#include <type_traits>

using namespace cs241c;
using namespace std;

using InsTyT = InstructionType;
using ValTyT = ValueType;

struct InsTyTInfo {
  const InsTyT InstrTy;
  const InstructionSignature Def;
  const string_view Mnemonic;
};

static const InsTyTInfo Types[] = {{InsTyT::Neg, {ValTyT::Value, {ValTyT::Value, ValTyT::Undef}}, "neg"},
                                   {InsTyT::Add, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "add"},
                                   {InsTyT::Sub, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "sub"},
                                   {InsTyT::Mul, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "mul"},
                                   {InsTyT::Div, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "div"},
                                   {InsTyT::Cmp, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "cmp"},
                                   {InsTyT::Adda, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "adda"},
                                   {InsTyT::Load, {ValTyT::Value, {ValTyT::Value, ValTyT::Undef}}, "load"},
                                   {InsTyT::Store, {ValTyT::Unit, {ValTyT::Value, ValTyT::Value}}, "store"},
                                   {InsTyT::Load, {ValTyT::Value, {ValTyT::StackSlot, ValTyT::Undef}}, "loadS"},
                                   {InsTyT::Store, {ValTyT::Unit, {ValTyT::Value, ValTyT::StackSlot}}, "storeS"},
                                   {InsTyT::Move, {ValTyT::Unit, {ValTyT::Value, ValTyT::Variable}}, "move"},
                                   {InsTyT::Phi, {ValTyT::Value, {ValTyT::Value, ValTyT::Value}}, "phi"},
                                   {InsTyT::End, {ValTyT::Unit, {ValTyT::Undef, ValTyT::Undef}}, "end"},
                                   {InsTyT::Bra, {ValTyT::Unit, {ValTyT::BasicBlock, ValTyT::Undef}}, "bra"},
                                   {InsTyT::Bne, {ValTyT::Unit, {ValTyT::Value, ValTyT::BasicBlock}}, "bne"},
                                   {InsTyT::Beq, {ValTyT::Unit, {ValTyT::Value, ValTyT::BasicBlock}}, "beq"},
                                   {InsTyT::Ble, {ValTyT::Unit, {ValTyT::Value, ValTyT::BasicBlock}}, "ble"},
                                   {InsTyT::Blt, {ValTyT::Unit, {ValTyT::Value, ValTyT::BasicBlock}}, "blt"},
                                   {InsTyT::Bge, {ValTyT::Unit, {ValTyT::Value, ValTyT::BasicBlock}}, "bge"},
                                   {InsTyT::Bgt, {ValTyT::Unit, {ValTyT::Value, ValTyT::BasicBlock}}, "bgt"},
                                   {InsTyT::Param, {ValTyT::Unit, {ValTyT::Value, ValTyT::Undef}}, "param"},
                                   {InsTyT::Call, {ValTyT::Value, {ValTyT::Function, ValTyT::Constant}}, "call"},
                                   {InsTyT::Ret, {ValTyT::Unit, {ValTyT::Any, ValTyT::Undef}}, "ret"},
                                   {InsTyT::Read, {ValTyT::Value, {ValTyT::Undef, ValTyT::Undef}}, "read"},
                                   {InsTyT::Write, {ValTyT::Unit, {ValTyT::Value, ValTyT::Undef}}, "write"},
                                   {InsTyT::WriteNL, {ValTyT::Unit, {ValTyT::Undef, ValTyT::Undef}}, "writeNL"},
                                   {InsTyT::Kill, {ValTyT::Unit, {ValTyT::Undef, ValTyT::Undef}}, "kill"}};

static const InsTyTInfo &typeInfo(InsTyT InstrTy) {
  auto Id = static_cast<underlying_type<InsTyT>::type>(InstrTy);
  return Types[Id];
}

const InstructionSignature &cs241c::signature(InsTyT InstrTy) { return typeInfo(InstrTy).Def; }

ValTyT cs241c::valTy(InsTyT InstrTy) { return typeInfo(InstrTy).Def.ValTy; }

string_view cs241c::mnemonic(InsTyT InstrTy) { return typeInfo(InstrTy).Mnemonic; }

bool cs241c::isConditionalBranch(InstructionType InsTy) {
  static const InstructionType Types[] = {InsTyT::Bne, InsTyT::Beq, InsTyT::Ble, InsTyT::Blt, InsTyT::Bge, InsTyT::Bgt};
  return find(begin(Types), end(Types), InsTy) != end(Types);
}

bool cs241c::isTerminator(InstructionType InsTy) {
  static const InstructionType Types[] = {InsTyT::End, InsTyT::Bra, InsTyT::Ret};
  return find(begin(Types), end(Types), InsTy) != end(Types) || isConditionalBranch(InsTy);
}

bool cs241c::isMemoryAccess(InstructionType InsTy) {
  static const InstructionType Types[] = {InsTyT::Load, InsTyT::Store};
  return find(begin(Types), end(Types), InsTy) != end(Types) || isConditionalBranch(InsTy);
}
