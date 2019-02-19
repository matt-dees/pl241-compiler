#ifndef CS241C_IR_INSTRUCTIONTYPE_H
#define CS241C_IR_INSTRUCTIONTYPE_H

#include "Value.h"
#include <string_view>

namespace cs241c {
enum class InstructionType {
  Neg,
  Add,
  Sub,
  Mul,
  Div,
  Cmp,

  Adda,
  Load,
  Store,
  Move,
  Phi,

  End,
  Bra,
  Bne,
  Beq,
  Ble,
  Blt,
  Bge,
  Bgt,

  Param,
  Call,
  Ret,

  Read,
  Write,
  WriteNL
};

struct InstructionDefinition {
  const ValueType ValTy;
  const ValueType Arg1;
  const ValueType Arg2;
};

const InstructionDefinition &instructionDefinition(InstructionType);
ValueType valTy(InstructionType);
std::string_view mnemonic(InstructionType);
} // namespace cs241c

#endif
