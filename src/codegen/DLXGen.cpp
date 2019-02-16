#include "DLXGen.h"
#include "Module.h"
#include <algorithm>
#include <array>
#include <unordered_map>
#include <vector>

using namespace cs241c;
using namespace std;

namespace {
namespace Reg {
const uint8_t R0 = 0;
const uint8_t FP = 28;
const uint8_t SP = 29;
const uint8_t GR = 30;
const uint8_t RA = 31;
} // namespace Reg

namespace Op {
// Arithmetic
const uint8_t ADD = 0;
const uint8_t SUB = 1;
const uint8_t MUL = 2;
const uint8_t DIV = 3;
const uint8_t MOD = 4;
const uint8_t CMP = 5;
const uint8_t OR = 8;
const uint8_t AND = 9;
const uint8_t BIC = 10;
const uint8_t XOR = 11;

const uint8_t LSH = 12;
const uint8_t ASH = 13;

const uint8_t CHK = 14;

const uint8_t ADDI = 16;
const uint8_t SUBI = 17;
const uint8_t MULI = 18;
const uint8_t DIVI = 29;
const uint8_t MODI = 20;
const uint8_t CMPI = 21;
const uint8_t ORI = 24;
const uint8_t ANDI = 25;
const uint8_t BICI = 26;
const uint8_t XORI = 27;

const uint8_t LSHI = 28;
const uint8_t ASHI = 29;

const uint8_t CHKI = 30;

// Load/Store
const uint8_t LDW = 32;
const uint8_t LDX = 33;
const uint8_t POP = 34;
const uint8_t STW = 36;
const uint8_t STX = 37;
const uint8_t PSH = 38;

// Control
const uint8_t BEQ = 40;
const uint8_t BNE = 41;
const uint8_t BLT = 42;
const uint8_t BGE = 43;
const uint8_t BLE = 44;
const uint8_t BGT = 45;

const uint8_t BSR = 46;
const uint8_t JSR = 48;
const uint8_t RET = 49;

// Input/Output
const uint8_t RDD = 50;
const uint8_t WRD = 51;
const uint8_t WRH = 52;
const uint8_t WRL = 53;
} // namespace Op

struct DLXObject {
  unordered_map<const GlobalVariable *, int32_t> GlobalVarOffsets;

  vector<uint8_t> CodeSegment;
  unordered_map<const Function *, int32_t> FunctionAddresses;

  DLXObject() { CodeSegment.reserve(10000000); }

  void addGlobals(const vector<unique_ptr<GlobalVariable>> &Globals) {
    int32_t Offset = 0;
    for (auto &Global : Globals) {
      Offset += Global->wordCount() * 4;
      GlobalVarOffsets[Global.get()] = Offset;
    }
  }

  void emitF1(uint8_t Op, uint8_t A, uint8_t B, int16_t C) {
    array<uint8_t, 4> Word;
    Word[0] = Op << 2 | ((A >> 3) & 0x3);
    Word[1] = A << 5 | (B & 0x1F);
    Word[2] = C >> 8;
    Word[3] = C;
    copy(Word.begin(), Word.end(), back_inserter(CodeSegment));
  }

  void emitF2(uint8_t Op, uint8_t A, uint8_t B, uint8_t C) {
    array<uint8_t, 4> Word;
    Word[0] = Op << 2 | ((A >> 3) & 0x3);
    Word[1] = A << 5 | (B & 0x1F);
    Word[2] = 0;
    Word[3] = (C & 0x1F);
    copy(Word.begin(), Word.end(), back_inserter(CodeSegment));
  }

  void emitF3(uint8_t Op, uint32_t C) {
    array<uint8_t, 4> Word;
    Word[0] = Op << 2 | ((C >> 24) & 0x3);
    Word[1] = C >> 16;
    Word[2] = C >> 8;
    Word[3] = C;
    copy(Word.begin(), Word.end(), back_inserter(CodeSegment));
  }

  void addFunction(Function *F) {
    int32_t Address = CodeSegment.size();
    FunctionAddresses[F] = Address;

    unordered_map<LocalVariable *, int16_t> LocalOffsets;

    // Prolog
    emitF1(Op::PSH, Reg::RA, Reg::SP, -4);
    emitF1(Op::PSH, Reg::FP, Reg::SP, -4);
    emitF1(Op::ADDI, Reg::FP, Reg::SP, 4);
    // Potentially safe registers

    // Make space for locals
    int16_t LocalOffset = 0;
    for (auto &Local : F->locals()) {
      LocalOffset -= Local->wordCount() * 4;
      LocalOffsets[Local.get()] = LocalOffset;
    }
    emitF1(Op::SUBI, Reg::SP, Reg::SP, -LocalOffset);

    // Do all the things

    // Epilog
    emitF1(Op::SUBI, Reg::SP, Reg::FP, 4);
    emitF1(Op::POP, Reg::FP, Reg::SP, 4);
    emitF1(Op::POP, Reg::RA, Reg::SP, 4);
    emitF2(Op::RET, 0, 0, Reg::RA);
  }

  void addFunctions(vector<unique_ptr<Function>> &Functions) {
    for (auto &FPtr : Functions) {
      addFunction(FPtr.get());
    }
  }
};
} // namespace

vector<uint8_t> cs241c::genDlx(Module &M) {
  DLXObject DLXO;
  DLXO.addGlobals(M.globals());
  DLXO.addFunctions(M.functions());
  return move(DLXO.CodeSegment);
}
