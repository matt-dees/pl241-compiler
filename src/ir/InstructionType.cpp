#include "InstructionType.h"
#include <array>
#include <type_traits>

using namespace cs241c;
using namespace std;

static array<string_view, 23> Mnemonics{"neg",   "add",  "sub", "mul",  "div",  "cmp",   "adda",   "load",
                                        "store", "move", "phi", "end",  "bra",  "bne",   "beq",    "ble",
                                        "blt",   "bge",  "bgt", "call", "read", "write", "writeNL"};

string_view cs241c::mnemonic(InstructionType InstrT) {
  auto Id = static_cast<underlying_type<InstructionType>::type>(InstrT);
  return Mnemonics[Id];
}
