#ifndef CS241C_IR_INSTRUCTIONTYPE_H
#define CS241C_IR_INSTRUCTIONTYPE_H

#include "Value.h"
#include <array>
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
  LoadS,
  StoreS,
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
  WriteNL,

  Kill
};

struct InstructionSignature {
  const ValueType ValTy;
  const std::array<ValueType, 2> Args;
};

const InstructionSignature &signature(InstructionType);
ValueType valTy(InstructionType);
std::string_view mnemonic(InstructionType);

bool isConditionalBranch(InstructionType);
bool isTerminator(InstructionType);
bool isMemoryAccess(InstructionType);
} // namespace cs241c

#endif
