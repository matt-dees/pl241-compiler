#include "DLXGen.h"
#include "FunctionAnalyzer.h"
#include "Module.h"
#include <algorithm>
#include <array>
#include <stack>
#include <unordered_map>
#include <vector>

using namespace cs241c;
using namespace std;

namespace {
enum Reg : uint8_t { R0 = 0, FP = 28, SP = 29, GR = 30, RA = 31 };

enum class Op : uint8_t {
  // Arithmetic
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
  DIVI = 29,
  MODI = 20,
  CMPI = 21,
  ORI = 24,
  ANDI = 25,
  BICI = 26,
  XORI = 27,

  LSHI = 28,
  ASHI = 29,

  CHKI = 30,

  // Load/Store
  LDW = 32,
  LDX = 33,
  POP = 34,
  STW = 36,
  STX = 37,
  PSH = 38,

  // Control
  BEQ = 40,
  BNE = 41,
  BLT = 42,
  BGE = 43,
  BLE = 44,
  BGT = 45,

  BSR = 46,
  JSR = 48,
  RET = 49,

  // Input/Output
  RDD = 50,
  WRD = 51,
  WRH = 52,
  WRL = 53,
};

constexpr uint8_t number(Op O) { return static_cast<uint8_t>(O); }

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

  void emitF1(Op Op, uint8_t A, uint8_t B, int16_t C) {
    array<uint8_t, 4> Word;
    Word[0] = number(Op) << 2 | ((A >> 3) & 0x3);
    Word[1] = A << 5 | (B & 0x1F);
    Word[2] = C >> 8;
    Word[3] = static_cast<uint8_t>(C);
    copy(Word.begin(), Word.end(), back_inserter(CodeSegment));
  }

  void emitF2(Op Op, uint8_t A, uint8_t B, uint8_t C) {
    array<uint8_t, 4> Word;
    Word[0] = number(Op) << 2 | ((A >> 3) & 0x3);
    Word[1] = A << 5 | (B & 0x1F);
    Word[2] = 0;
    Word[3] = (C & 0x1F);
    copy(Word.begin(), Word.end(), back_inserter(CodeSegment));
  }

  void emitF3(Op Op, uint32_t C) {
    array<uint8_t, 4> Word;
    Word[0] = number(Op) << 2 | ((C >> 24) & 0x3);
    Word[1] = C >> 16;
    Word[2] = C >> 8;
    Word[3] = C;
    copy(Word.begin(), Word.end(), back_inserter(CodeSegment));
  }

  int mapSpills(Function &F, FunctionAnalyzer &FA, unordered_map<Value *, int16_t> &ValueOffsets, int StartOffset) {
    auto Registers = FA.coloring(&F);

    int CurrentOffset = StartOffset;
    for (auto &BB : F.basicBlocks()) {
      for (auto &Instr : BB->instructions()) {
        if (isSubtype(Instr->ValTy, ValueType::Value)) {
          if (Registers->find(Instr.get()) == Registers->end()) {
            CurrentOffset -= 4;
            ValueOffsets[Instr.get()] = CurrentOffset;
          }
        }
      }
    }
    return CurrentOffset;
  }

  vector<BasicBlock *> linearize(Function *F) {
    stack<BasicBlock *> JumpBlocks;
    vector<BasicBlock *> LinearBlockOrder;

    BasicBlock *CurrentBB = F->entryBlock();

    while (CurrentBB != nullptr) {
      LinearBlockOrder.push_back(CurrentBB);

      if (CurrentBB->fallthoughSuccessor() != nullptr) {
        if (dynamic_cast<ConditionalBlockTerminator *>(CurrentBB->terminator())) {
          JumpBlocks.push(CurrentBB);
        }

        CurrentBB = CurrentBB->fallthoughSuccessor();
      } else {
        if (!JumpBlocks.empty()) {
          CurrentBB = JumpBlocks.top();
          JumpBlocks.pop();
          CurrentBB = CurrentBB->terminator()->target();
        } else {
          CurrentBB = nullptr;
        }
      }
    }

	return LinearBlockOrder;
  }

  void addFunction(Function *F, FunctionAnalyzer &FA) {
    int32_t Address = static_cast<int32_t>(CodeSegment.size());
    FunctionAddresses[F] = Address;

    // Prolog
    emitF1(Op::PSH, FP, SP, -4);
    emitF1(Op::PSH, RA, SP, -4);
    emitF1(Op::ADDI, FP, SP, 4);

    // Sage registers 1-8
    for (uint8_t R = 1; R <= 8; ++R) {
      emitF1(Op::PSH, R, SP, -4);
    }

    // Make space for arrays
    unordered_map<Value *, int16_t> ValueOffsets;
    int Offset = -4;

    for (auto &Local : F->locals()) {
      if (!Local->isSingleWord()) {
        Offset -= 4 * Local->wordCount();
        ValueOffsets[Local.get()] = Offset;
      }
    }

    // Make space for spills
    Offset = mapSpills(*F, FA, ValueOffsets, Offset);
    emitF1(Op::ADDI, SP, FP, Offset);

    // Process all basic blocks
    auto Blocks = linearize(F);

    // Epilog
    emitF1(Op::ADDI, SP, FP, -4);
    emitF1(Op::POP, RA, SP, 4);
    emitF1(Op::POP, FP, SP, 4);
    emitF2(Op::RET, 0, 0, RA);
  }

  void addFunctions(vector<unique_ptr<Function>> &Functions, FunctionAnalyzer &FA) {
    for (auto &FPtr : Functions) {
      addFunction(FPtr.get(), FA);
    }
  }
};
} // namespace

vector<uint8_t> cs241c::genDlx(Module &M, FunctionAnalyzer &FA) {
  DLXObject DLXO;
  DLXO.addGlobals(M.globals());
  DLXO.addFunctions(M.functions(), FA);
  return move(DLXO.CodeSegment);
}
