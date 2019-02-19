#ifndef CS241C_IR_INSTRUCTIONTYPE_H
#define CS241C_IR_INSTRUCTIONTYPE_H

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

  Call,
  Ret,

  Read,
  Write,
  WriteNL
};

std::string_view mnemonic(InstructionType);
} // namespace cs241c

#endif
