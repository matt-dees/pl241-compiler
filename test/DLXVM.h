#ifndef CS241C_TEST_DLXVM_H
#define CS241C_TEST_DLXVM_H

#include <array>
#include <cstdint>
#include <string>

class DLXVM {
  enum class OpFormat { F1, F2, F3 };
  enum class OpCode : int32_t {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    MOD = 4,
    CMP = 5,
    OR = 8,
    AND = 9,
    BIC = 10,
    XOR = 11,
    LSH = 12,
    ASH = 13,
    CHK = 14,

    ADDI = 16,
    SUBI = 17,
    MULI = 18,
    DIVI = 19,
    MODI = 20,
    CMPI = 21,
    ORI = 24,
    ANDI = 25,
    BICI = 26,
    XORI = 27,
    LSHI = 28,
    ASHI = 29,
    CHKI = 30,

    LDW = 32,
    LDX = 33,
    POP = 34,
    STW = 36,
    STX = 37,
    PSH = 38,

    BEQ = 40,
    BNE = 41,
    BLT = 42,
    BGE = 43,
    BLE = 44,
    BGT = 45,
    BSR = 46,
    JSR = 48,
    RET = 49,

    RDI = 50,
    WRD = 51,
    WRH = 52,
    WRL = 53,

    ERR = 63
  };

  static constexpr int32_t MemSize = 10000;
  std::array<uint32_t, MemSize / sizeof(uint32_t)> M{};

  int32_t PC = 0;
  std::array<int32_t, 32> R{};

  OpFormat Format{};
  OpCode Op{};
  int32_t A = 0;
  int32_t B = 0;
  int32_t C = 0;

public:
  DLXVM();
  void execute();

private:
  void loadInstr(uint32_t Word);
  std::string disassemble(uint32_t Word);
};

#endif
