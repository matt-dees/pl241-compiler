#include "DLXVM.h"
#include <iostream>

static const std::array<const char *, 64> OpCodeMnemo = {
    "ADD",  "SUB",  "MUL",  "DIV",  "MOD",  "CMP",  "ERR",  "ERR",
    "OR",   "AND",  "BIC",  "XOR",  "LSH",  "ASH",  "CHK",  "ERR",
    "ADDI", "SUBI", "MULI", "DIVI", "MODI", "CMPI", "ERRI", "ERRI",
    "ORI",  "ANDI", "BICI", "XORI", "LSHI", "ASHI", "CHKI", "ERR",
    "LDW",  "LDX",  "POP",  "ERR",  "STW",  "STX",  "PSH",  "ERR",
    "BEQ",  "BNE",  "BLT",  "BGE",  "BLE",  "BGT",  "BSR",  "ERR",
    "JSR",  "RET",  "RDI",  "WRD",  "WRH",  "WRL",  "ERR",  "ERR",
    "ERR",  "ERR",  "ERR",  "ERR",  "ERR",  "ERR",  "ERR",  "ERR"};

void bug(int N);

DLXVM::DLXVM() { R[30] = MemSize - 1; }

void DLXVM::execute() {
  int32_t RetC = 0;

  while (true) {
    R[0] = 0;
    loadInstr(M[PC]);

    int nextPC = PC + 1;
    if (Format == OpFormat::F2) {
      RetC = C;
      C = R[C];
    }

    switch (Op) {
    case OpCode::ADD:
    case OpCode::ADDI:
      R[A] = R[B] + C;
      break;
    case OpCode::SUB:
    case OpCode::SUBI:
      R[A] = R[B] - C;
      break;
    case OpCode::CMP:
    case OpCode::CMPI:
      R[A] = R[B] - C;
      if (R[A] < 0)
        R[A] = -1;
      else if (R[A] > 0)
        R[A] = 1;
      break;
    case OpCode::MUL:
    case OpCode::MULI:
      R[A] = R[B] * C;
      break;
    case OpCode::DIV:
    case OpCode::DIVI:
      R[A] = R[B] / C;
      break;
    case OpCode::MOD:
    case OpCode::MODI:
      R[A] = R[B] % C;
      break;
    case OpCode::OR:
    case OpCode::ORI:
      R[A] = R[B] | C;
      break;
    case OpCode::AND:
    case OpCode::ANDI:
      R[A] = R[B] & C;
      break;
    case OpCode::BIC:
    case OpCode::BICI:
      R[A] = R[B] & ~C;
      break;
    case OpCode::XOR:
    case OpCode::XORI:
      R[A] = R[B] ^ C;
      break;
    case OpCode::LSH:
    case OpCode::LSHI:
      if ((C < -31) || (C > 31)) {
        std::cout << "Illegal value " << C << " of operand c or register c!\n";
        bug(1);
      }
      if (C < 0)
        R[A] = R[B] >> -C;
      else
        R[A] = R[B] << C;
      break;
    case OpCode::ASH:
    case OpCode::ASHI:
      if ((C < -31) || (C > 31)) {
        std::cout << "DLX.execute: Illegal value " << C
                  << " of operand c or register c!\n";
        bug(1);
      }
      if (C < 0)
        R[A] = R[B] >> -C;
      else
        R[A] = R[B] << C;
      break;
    case OpCode::CHKI:
    case OpCode::CHK:
      if (R[A] < 0) {
        std::cout << "DLX.execute: " << PC * 4 << ": R[" << A << "] == " << R[A]
                  << " < 0\n";
        bug(14);
      } else if (R[A] >= C) {
        std::cout << "DLX.execute: " << PC * 4 << ": R[" << A << "] == " << R[A]
                  << " >= " << C << "\n";
        bug(14);
      }
      break;
    case OpCode::LDW:
    case OpCode::LDX:
      R[A] = M[(R[B] + C) / 4];
      break;
    case OpCode::STW:
    case OpCode::STX:
      M[(R[B] + C) / 4] = static_cast<uint32_t>(R[A]);
      break;
    case OpCode::POP:
      R[A] = M[R[B] / 4];
      R[B] = R[B] + C;
      break;
    case OpCode::PSH:
      R[B] = R[B] + C;
      M[R[B] / 4] = static_cast<unsigned int>(R[A]);
      break;
    case OpCode::BEQ:
      if (R[A] == 0)
        nextPC = PC + C;
      if ((nextPC < 0) || (nextPC > MemSize / 4)) {
        std::cout << 4 * nextPC << " is no address in memory (0.." << MemSize
                  << ").\n";
        bug(40);
      }
      break;
    case OpCode::BNE:
      if (R[A] != 0)
        nextPC = PC + C;
      if ((nextPC < 0) || (nextPC > MemSize / 4)) {
        std::cout << 4 * nextPC << " is no address in memory (0.." << MemSize
                  << ").\n";
        bug(41);
      }
      break;
    case OpCode::BLT:
      if (R[A] < 0)
        nextPC = PC + C;
      if ((nextPC < 0) || (nextPC > MemSize / 4)) {
        std::cout << 4 * nextPC << " is no address in memory (0.." << MemSize
                  << ").\n";
        bug(42);
      }
      break;
    case OpCode::BGE:
      if (R[A] >= 0)
        nextPC = PC + C;
      if ((nextPC < 0) || (nextPC > MemSize / 4)) {
        std::cout << 4 * nextPC << " is no address in memory (0.." << MemSize
                  << ").\n";
        bug(43);
      }
      break;
    case OpCode::BLE:
      if (R[A] <= 0)
        nextPC = PC + C;
      if ((nextPC < 0) || (nextPC > MemSize / 4)) {
        std::cout << 4 * nextPC << " is no address in memory (0.." << MemSize
                  << ").\n";
        bug(44);
      }
      break;
    case OpCode::BGT:
      if (R[A] > 0)
        nextPC = PC + C;
      if ((nextPC < 0) || (nextPC > MemSize / 4)) {
        std::cout << 4 * nextPC << " is no address in memory (0.." << MemSize
                  << ").\n";
        bug(45);
      }
      break;
    case OpCode::BSR:
      R[31] = (PC + 1) * 4;
      nextPC = PC + C;
      break;
    case OpCode::JSR:
      R[31] = (PC + 1) * 4;
      nextPC = C / 4;
      break;
    case OpCode::RET:
      if (RetC == 0)
        return;
      if ((C < 0) || (C > MemSize)) {
        std::cout << C << " is no address in memory (0.." << MemSize << ").\n";
        bug(49);
      }
      nextPC = C / 4;
      break;
    case OpCode::RDI:
      std::cout << "?: ";
      std::cin >> R[A];
      break;
    case OpCode::WRD:
      std::cout << R[B] << "  ";
      break;
    case OpCode::WRH:
      std::cout << "0x" << std::hex << R[B] << "  ";
      break;
    case OpCode::WRL:
      std::cout << "\n";
      break;
    case OpCode::ERR:
      std::cout << "Program dropped off the end!";
      bug(1);
      break;
    default:
      std::cout << "DLX.execute: Unknown opcode encountered!";
      bug(1);
    }
    PC = nextPC;
  }
}

void DLXVM::loadInstr(uint32_t Word) {
  Op = static_cast<OpCode>(Word >> 26);

  switch (Op) {
  case OpCode::BSR:
  case OpCode::RDI:
  case OpCode::WRD:
  case OpCode::WRH:
  case OpCode::WRL:
  case OpCode::CHKI:
  case OpCode::BEQ:
  case OpCode::BNE:
  case OpCode::BLT:
  case OpCode::BGE:
  case OpCode::BLE:
  case OpCode::BGT:
  case OpCode::ADDI:
  case OpCode::SUBI:
  case OpCode::MULI:
  case OpCode::DIVI:
  case OpCode::MODI:
  case OpCode::CMPI:
  case OpCode::ORI:
  case OpCode::ANDI:
  case OpCode::BICI:
  case OpCode::XORI:
  case OpCode::LSHI:
  case OpCode::ASHI:
  case OpCode::LDW:
  case OpCode::POP:
  case OpCode::STW:
  case OpCode::PSH:
    Format = OpFormat::F1;
    A = (Word >> 21) & 0x1F;
    B = (Word >> 16) & 0x1F;
    C = static_cast<int16_t>(Word & 0xFF);
    break;
  case OpCode::RET:
  case OpCode::CHK:
  case OpCode::ADD:
  case OpCode::SUB:
  case OpCode::MUL:
  case OpCode::DIV:
  case OpCode::MOD:
  case OpCode::CMP:
  case OpCode::OR:
  case OpCode::AND:
  case OpCode::BIC:
  case OpCode::XOR:
  case OpCode::LSH:
  case OpCode::ASH:
  case OpCode::LDX:
  case OpCode::STX:
    Format = OpFormat::F2;
    A = (Word >> 21) & 0x1F;
    B = (Word >> 16) & 0x1F;
    C = Word & 0x1F;
    break;
  case OpCode::JSR:
    Format = OpFormat::F3;
    A = -1;
    B = -1;
    C = Word & 0x3FFFFFF;
    break;
  default:
    std::cout << "Illegal instruction! (" << PC << ")";
  }
}

std::string DLXVM::disassemble(uint32_t Word) {
  loadInstr(Word);
  std::string Line = std::string{OpCodeMnemo[static_cast<int32_t>(Op)]} + "  ";

  switch (Op) {
  case OpCode::WRL:
    return Line += "\n";
  case OpCode::BSR:
  case OpCode::RET:
  case OpCode::JSR:
    return Line + std::to_string(C) + "\n";
  case OpCode::RDI:
    return Line + std::to_string(A) + "\n";
  case OpCode::WRD:
  case OpCode::WRH:
    return Line + std::to_string(B) + "\n";
  case OpCode::CHKI:
  case OpCode::BEQ:
  case OpCode::BNE:
  case OpCode::BLT:
  case OpCode::BGE:
  case OpCode::BLE:
  case OpCode::BGT:
  case OpCode::CHK:
    return Line + std::to_string(A) + " " + std::to_string(C) + "\n";
  case OpCode::ADDI:
  case OpCode::SUBI:
  case OpCode::MULI:
  case OpCode::DIVI:
  case OpCode::MODI:
  case OpCode::CMPI:
  case OpCode::ORI:
  case OpCode::ANDI:
  case OpCode::BICI:
  case OpCode::XORI:
  case OpCode::LSHI:
  case OpCode::ASHI:
  case OpCode::LDW:
  case OpCode::POP:
  case OpCode::STW:
  case OpCode::PSH:
  case OpCode::ADD:
  case OpCode::SUB:
  case OpCode::MUL:
  case OpCode::DIV:
  case OpCode::MOD:
  case OpCode::CMP:
  case OpCode::OR:
  case OpCode::AND:
  case OpCode::BIC:
  case OpCode::XOR:
  case OpCode::LSH:
  case OpCode::ASH:
  case OpCode::LDX:
  case OpCode::STX:
    return Line + std::to_string(A) + " " + std::to_string(B) + " " +
           std::to_string(C) + "\n";
  default:
    return Line + "\n";
  }
}

void bug(int N) {
  std::cout << "bug number: " << N;
  std::cin.get();
  exit(N);
}
