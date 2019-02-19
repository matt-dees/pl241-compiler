#include "InstructionType.h"
#include <array>
#include <type_traits>

using namespace cs241c;
using namespace std;

using InsTyT = InstructionType;
using ValTyT = ValueType;

struct InsTyTInfo {
  const InsTyT InstrTy;
  const InstructionDefinition Def;
  const string_view Mnemonic;
};

static const array<InsTyTInfo, 25> Types{{{InsTyT::Neg, {ValTyT::Value, ValTyT::Value, ValTyT::Undef}, "neg"},
                                          {InsTyT::Add, {ValTyT::Value, ValTyT::Value, ValTyT::Value}, "add"},
                                          {InsTyT::Sub, {ValTyT::Value, ValTyT::Value, ValTyT::Value}, "sub"},
                                          {InsTyT::Mul, {ValTyT::Value, ValTyT::Value, ValTyT::Value}, "mul"},
                                          {InsTyT::Div, {ValTyT::Value, ValTyT::Value, ValTyT::Value}, "div"},
                                          {InsTyT::Cmp, {ValTyT::Cmp, ValTyT::Value, ValTyT::Value}, "cmp"},
                                          {InsTyT::Adda, {ValTyT::Adda, ValTyT::Value, ValTyT::Value}, "adda"},
                                          {InsTyT::Load, {ValTyT::Value, ValTyT::Value, ValTyT::Undef}, "load"},
                                          {InsTyT::Store, {ValTyT::None, ValTyT::Value, ValTyT::Value}, "store"},
                                          {InsTyT::Move, {ValTyT::None, ValTyT::Value, ValTyT::Value}, "move"},
                                          {InsTyT::Phi, {ValTyT::Value, ValTyT::Value, ValTyT::Value}, "phi"},
                                          {InsTyT::End, {ValTyT::None, ValTyT::Undef, ValTyT::Undef}, "end"},
                                          {InsTyT::Bra, {ValTyT::None, ValTyT::BasicBlock, ValTyT::Undef}, "bra"},
                                          {InsTyT::Bne, {ValTyT::None, ValTyT::Cmp, ValTyT::BasicBlock}, "bne"},
                                          {InsTyT::Beq, {ValTyT::None, ValTyT::Cmp, ValTyT::BasicBlock}, "beq"},
                                          {InsTyT::Ble, {ValTyT::None, ValTyT::Cmp, ValTyT::BasicBlock}, "ble"},
                                          {InsTyT::Blt, {ValTyT::None, ValTyT::Cmp, ValTyT::BasicBlock}, "blt"},
                                          {InsTyT::Bge, {ValTyT::None, ValTyT::Cmp, ValTyT::BasicBlock}, "bge"},
                                          {InsTyT::Bgt, {ValTyT::None, ValTyT::Cmp, ValTyT::BasicBlock}, "bgt"},
                                          {InsTyT::Param, {ValTyT::None, ValTyT::Value, ValTyT::Undef}, "param"},
                                          {InsTyT::Call, {ValTyT::Value, ValTyT::Function, ValTyT::Constant}, "call"},
                                          {InsTyT::Ret, {ValTyT::None, ValTyT::Any, ValTyT::Undef}, "ret"},
                                          {InsTyT::Read, {ValTyT::Value, ValTyT::Undef, ValTyT::Undef}, "read"},
                                          {InsTyT::Write, {ValTyT::None, ValTyT::Value, ValTyT::Undef}, "write"},
                                          {InsTyT::WriteNL, {ValTyT::None, ValTyT::Undef, ValTyT::Undef}, "writeNL"}}};

static const InsTyTInfo &typeInfo(InsTyT InstrTy) {
  auto Id = static_cast<underlying_type<InsTyT>::type>(InstrTy);
  return Types[Id];
}

const InstructionDefinition &cs241c::instructionDefinition(InsTyT InstrTy) { return typeInfo(InstrTy).Def; }

ValTyT cs241c::valTy(InsTyT InstrTy) { return typeInfo(InstrTy).Def.ValTy; }

string_view cs241c::mnemonic(InsTyT InstrTy) { return typeInfo(InstrTy).Mnemonic; }
