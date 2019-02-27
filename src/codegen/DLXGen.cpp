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

struct DLXGenState {
  unordered_map<Value *, int16_t> &ValueOffsets;
  FunctionAnalyzer &FA;
  Function *CurrentFunction;
};

enum Reg : uint8_t {
  R0 = 0,
  R1,
  R2,
  R3,
  R4,
  R5,
  R6,
  R7,
  R8,
  Accu = 9,
  Spill1 = 10,
  Spill2 = 11,
  FP = 28,
  SP = 29,
  GR = 30,
  RA = 31
};

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

  int32_t addGlobals(const vector<unique_ptr<GlobalVariable>> &Globals) {
    int32_t Offset = 0;
    for (auto &Global : Globals) {
      Offset += Global->wordCount() * 4;
      GlobalVarOffsets[Global.get()] = Offset;
    }
    return Offset;
  }

  Reg mapValueToRegister(ValueRef Val, DLXGenState &State, Reg SpillReg) {
    if (Val.ValTy == ValueType::Register) {
      return static_cast<Reg>(Val.R.Id);
    }
    return State.FA.isValueSpilled(State.CurrentFunction, Val)
               ? SpillReg
               : static_cast<Reg>(State.FA.coloring(State.CurrentFunction)->at(Val));
  }

  void prepareConstantRegister(Reg Register, int Value) { emitF1(Op::ADDI, Register, Reg::R0, Value); }

  void prepareSpilledRegister(Reg Regster, int Offset) { emitF1(Op::LDW, Regster, Reg::FP, Offset); }

  void restoreSpilledRegister(Reg Register, int Offset) { emitF1(Op::STW, Register, FP, Offset); }

  Reg prepareOperandRegister(ValueRef Val, DLXGenState &State, Reg SpillRegToUse) {
    Reg R = mapValueToRegister(Val, State, SpillRegToUse);
    if (Val.ValTy == ValueType::Constant) {
      prepareConstantRegister(R, dynamic_cast<ConstantValue *>((Value *)Val)->Val);
    } else if (R == SpillRegToUse) {
      prepareSpilledRegister(R, State.ValueOffsets.at(Val));
    }
    return R;
  }

  Op getArithmeticOpCode(Instruction &Instr) {
    if (Instr.arguments().size() != 2) {
      throw logic_error("Unable to emit arithmetic instruction " + Instr.toString() +
                        " because it does not have two arguments.");
    }

    bool const containsConstant = Instr.arguments().at(1).ValTy == ValueType::Constant;
    Op OpCode;
    switch (Instr.InstrT) {
    case InstructionType::Adda:
    case InstructionType::Add: {
      OpCode = Op::ADD;
      break;
    }
    case InstructionType::Sub: {
      OpCode = Op::SUB;
      break;
    }
    case InstructionType::Mul: {
      OpCode = Op::MUL;
      break;
    }
    case InstructionType::Div: {
      OpCode = Op::DIV;
      break;
    }
    case InstructionType::Cmp: {
      OpCode = Op::CMP;
      break;
    }
    default:
      throw logic_error("Unsupported arithmetic instruction");
    }

    if (containsConstant) {
      // Immediate variants are offset by 16
      OpCode = static_cast<Op>(number(OpCode) + number(Op::ADDI));
    }
    return OpCode;
  }

  void emitArithmeticImmediate(Op OpCode, ValueRef A, ValueRef B, int32_t C, DLXGenState &State) {
    if (OpCode < Op::ADDI || OpCode > Op::CHKI) {
      throw logic_error("Invalid Opcode for immediate arithmetic emission");
    }
    if (OpCode == Op::CHKI) {
      throw logic_error("Unsupported for now");
    }
    Reg Ra = mapValueToRegister(A, State, Reg::Spill1);
    Reg Rb = prepareOperandRegister(B, State, Reg::Spill1);
    emitF1(OpCode, Ra, Rb, C);
    if (Ra == Reg::Spill1) {
      restoreSpilledRegister(Ra, State.ValueOffsets.at(A));
    }
  }

  void emitArithmeticRegister(Op OpCode, ValueRef A, ValueRef B, ValueRef C, DLXGenState &State) {
    if (OpCode > Op::CHK) {
      throw logic_error("Invalid Opcode for register arithmetic emission");
    }
    if (OpCode == Op::CHK) {
      throw logic_error("Unsupported for now");
    }
    Reg Ra = mapValueToRegister(A, State, Reg::Spill1);
    Reg Rb = prepareOperandRegister(B, State, Reg::Spill1);
    Reg Rc = prepareOperandRegister(C, State, Reg::Spill2);
    emitF1(OpCode, Ra, Rb, Rc);
    if (Ra == Reg::Spill1) {
      restoreSpilledRegister(Ra, State.ValueOffsets.at(A));
    }
  }

  void emitArithmetic(Instruction &Inst, DLXGenState &State) {

    auto Args = Inst.arguments();
    Op OpCode = getArithmeticOpCode(Inst);
    if (OpCode <= Op::CHK) {
      emitArithmeticRegister(OpCode, &Inst, Args.at(0), Args.at(1), State);
    } else {
      emitArithmeticImmediate(OpCode, &Inst, Args.at(0), dynamic_cast<ConstantValue *>((Value *)Args.at(1))->Val,
                              State);
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

  void setupRegisters(int32_t StackOffset) {
    emitF1(Op::SUBI, Reg::FP, Reg::GR, StackOffset);
    emitF2(Op::ADD, Reg::SP, Reg::FP, Reg::R0);
  }

  int mapSpills(Function &F, FunctionAnalyzer &FA, unordered_map<Value *, int16_t> &ValueOffsets, int StartOffset) {
    int CurrentOffset = StartOffset;
    for (auto &BB : F.basicBlocks()) {
      for (auto &Instr : BB->instructions()) {
        if (isSubtype(Instr->ValTy, ValueType::Value)) {
          if (FA.isValueSpilled(&F, Instr.get())) {
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
        if (CurrentBB->isTerminated() && isConditionalBranch(CurrentBB->terminator()->InstrT)) {
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

  void emitInstruction(Instruction &Instr, DLXGenState &State) {
    switch (Instr.InstrT) {
    case InstructionType::Neg: {
      emitArithmeticImmediate(Op::MULI, &Instr, Instr.arguments().at(0), -1, State);
      break;
    }
    case InstructionType::Add:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Sub:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Mul:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Div:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Cmp:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Adda:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Load:
      break;
    case InstructionType::Store:
      break;
    case InstructionType::Move:
      emitArithmeticImmediate(Op::ADDI, Instr.arguments().at(1), Instr.arguments().at(0), 0, State);
      break;
    case InstructionType::Phi:
      throw logic_error("Error: Unable to emit Phi instructions.");
      break;
    case InstructionType::End:
      emitF2(Op::RET, 0, 0, 0);
      break;
    case InstructionType::Bra:
      break;
    case InstructionType::Bne:
      break;
    case InstructionType::Beq:
      break;
    case InstructionType::Ble:
      break;
    case InstructionType::Blt:
      break;
    case InstructionType::Bge:
      break;
    case InstructionType::Bgt:
      break;
    case InstructionType::Param:
      break;
    case InstructionType::Call:
      break;
    case InstructionType::Ret:
      break;
    case InstructionType::Read: {
      Reg Ra = mapValueToRegister(&Instr, State, Reg::Accu);
      emitF2(Op::RDD, Ra, 0, 0);
      break;
    }
    case InstructionType::Write: {
      auto Arg = Instr.arguments()[0];
      Reg Rb = mapValueToRegister(Arg, State, Reg::Accu);
      if (Arg.ValTy == ValueType::Constant) {
        prepareConstantRegister(Rb, dynamic_cast<ConstantValue *>((Value *)Arg)->Val);
      }
      emitF2(Op::WRD, 0, Rb, 0);
      break;
    }
    case InstructionType::WriteNL:
      emitF1(Op::WRL, 0, 0, 0);
      break;
    }
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

    DLXGenState CurrentState = {ValueOffsets, FA, F};
    for (auto Block : Blocks) {
      for (auto &Instr : Block->instructions()) {
        emitInstruction(*Instr, CurrentState);
      }
    }

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
}; // namespace
} // namespace

vector<uint8_t> cs241c::genDlx(Module &M, FunctionAnalyzer &FA) {
  DLXObject DLXO;
  int32_t StackOffset = DLXO.addGlobals(M.globals());
  DLXO.setupRegisters(StackOffset);
  DLXO.addFunctions(M.functions(), FA);
  return move(DLXO.CodeSegment);
}
