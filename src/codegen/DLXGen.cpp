#include "DLXGen.h"
#include "FunctionAnalyzer.h"
#include "Module.h"
#include "RegisterAllocator.h"
#include <algorithm>
#include <array>
#include <limits>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace cs241c;
using namespace std;

namespace {

struct DLXGenState {
  unordered_map<RegisterAllocator::VirtualRegister, int32_t> SpilledRegisterOffsets;
  unordered_map<Value *, int32_t> ValueOffsets;
  FunctionAnalyzer &FA;
  Function *CurrentFunction;
  unordered_map<BasicBlock *, int32_t> BlockAddresses;
  vector<pair<BasicBlock *, int32_t>> Fixups;
  RegisterAllocator::VirtualRegister lookupRegisterOffset(ValueRef Val) {
    if (Val.ValTy == ValueType::Register) {
      return SpilledRegisterOffsets.at(Val.R.Id);
    }
    return SpilledRegisterOffsets.at(FA.coloring(CurrentFunction)->at(Val));
  }
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
      return State.FA.isRegisterSpilled(static_cast<Reg>(Val.R.Id)) ? SpillReg : static_cast<Reg>(Val.R.Id);
    } else if (Val->name() == "GlobalBase") {
      return Reg::GR;
    }
    return State.FA.isValueSpilled(State.CurrentFunction, Val)
               ? SpillReg
               : static_cast<Reg>(State.FA.coloring(State.CurrentFunction)->at(Val));
  }

  void prepareConstantRegister(Reg Register, int Value) {
    if (fitsIn16BitReg(Value)) {
      emitF1(Op::ADDI, Register, Reg::R0, Value);
      return;
    }
    throw logic_error("32-Bit values not supported yet.");
  }

  void loadStackValue(Reg Register, int Offset) { emitF1(Op::LDW, Register, Reg::FP, Offset); }

  void storeStackValue(Reg Register, int Offset) { emitF1(Op::STW, Register, FP, Offset); }

  Reg loadValueIntoRegister(ValueRef Val, DLXGenState &State, Reg SpillRegToUse) {
    Reg R = mapValueToRegister(Val, State, SpillRegToUse);

    if (Val.ValTy == ValueType::Constant) {
      prepareConstantRegister(R, dynamic_cast<ConstantValue *>((Value *)Val)->Val);
    } else if (Val.ValTy == ValueType::Variable) {
      if (auto Param = dynamic_cast<LocalVariable *>(Val.R.Ptr)) {
        loadStackValue(R, State.ValueOffsets.at(Param));
      } else {
        prepareConstantRegister(R, -1 * GlobalVarOffsets.at(dynamic_cast<GlobalVariable *>(Val.R.Ptr)));
      }
    } else if (R == SpillRegToUse) {
      loadStackValue(R, State.lookupRegisterOffset(Val));
    }
    return R;
  }

  Op getArithmeticOpCode(Instruction &Instr) {
    auto Args = Instr.arguments();

    if (Args.size() != 2) {
      throw logic_error("Unable to emit arithmetic instruction " + Instr.toString() +
                        " because it does not have two arguments.");
    }

    bool const containsConstant = Args.at(1).ValTy == ValueType::Constant;
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
    Reg Rb = loadValueIntoRegister(B, State, Reg::Spill1);
    emitF1(OpCode, Ra, Rb, C);
    if (Ra == Reg::Spill1) {
      storeStackValue(Ra, State.lookupRegisterOffset(A));
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
    Reg Rb = loadValueIntoRegister(B, State, Reg::Spill1);
    Reg Rc = loadValueIntoRegister(C, State, Reg::Spill2);
    emitF1(OpCode, Ra, Rb, Rc);
    if (Ra == Reg::Spill1) {
      storeStackValue(Ra, State.lookupRegisterOffset(A));
    }
  }

  bool isCommutative(Op OpCode) {
    return OpCode == Op::ADD || OpCode == Op::MUL || OpCode == Op::OR || OpCode == Op::AND || OpCode == Op::XOR;
  }

  void emitArithmetic(Instruction &Inst, DLXGenState &State) {
    auto Args = Inst.arguments();
    Op OpCode = getArithmeticOpCode(Inst);

    if (Args[0].ValTy == ValueType::Constant && isCommutative(OpCode)) {
      auto Arg1 = Args[1];
      Inst.updateArg(1, Args[0]);
      Inst.updateArg(0, Arg1);
      Args = Inst.arguments();
    }
    if (OpCode <= Op::CHK) {
      emitArithmeticRegister(OpCode, &Inst, Args.at(0), Args.at(1), State);
    } else {
      emitArithmeticImmediate(OpCode, &Inst, Args.at(0), dynamic_cast<ConstantValue *>((Value *)Args.at(1))->Val,
                              State);
    }
  }

  bool fitsIn16BitReg(int32_t Val) {
    return Val <= numeric_limits<int16_t>::max() && Val >= numeric_limits<int16_t>::min();
  }

  void emitMemory(Instruction &Instr, DLXGenState &State) {
    bool isLoad = Instr.InstrT == InstructionType::Load;
    if (!isLoad && Instr.InstrT != InstructionType::Store) {
      throw logic_error("Invalid instruction type for emitMemory().");
    }
    if (Instr.arguments().size() != (isLoad ? 1 : 2)) {
      throw logic_error("Incorrect number of arguments for memory instruction.");
    }
    // Adda will be at position 1 for Store, 0 for Load
    Instruction *Adda = dynamic_cast<Instruction *>((Value *)Instr.arguments().at((isLoad ? 0 : 1)));
    if (Adda == nullptr) {
      throw logic_error("Argument to Load was not ADDA.");
    }
    // Load destination is the instruction, Store destination is first param
    Reg const RaSpill = Reg::Accu;
    ValueRef AVal = isLoad ? &Instr : Instr.arguments().at(0);
    Reg Ra = isLoad ? mapValueToRegister(AVal, State, RaSpill) : loadValueIntoRegister(AVal, State, RaSpill);
    Reg Rb = loadValueIntoRegister(Adda->arguments().at(0), State, Reg::Spill1);
    if (Adda->arguments().at(1).ValTy == ValueType::Constant) {
      int32_t C = dynamic_cast<ConstantValue *>((Value *)Adda->arguments().at(1))->Val;
      emitF1(isLoad ? Op::LDW : Op::STW, Ra, Rb, C);
    } else {
      Reg Rc = loadValueIntoRegister(Adda->arguments().at(1), State, Reg::Spill2);
      emitF2(isLoad ? Op::LDX : Op::STX, Ra, Rb, Rc);
    }
    if (Ra == RaSpill && isLoad) {
      storeStackValue(RaSpill, State.lookupRegisterOffset(AVal));
    }
  }

  void emitBranch(Instruction &Instr, DLXGenState &State) {
    // Do the CmpReg up here or else you might calculate wrong addresses for the fixup!
    Reg CmpReg = Instr.InstrT == InstructionType::Bra
                     ? Reg::R0
                     : loadValueIntoRegister(Instr.arguments().at(0), State, Reg::Spill1);

    int32_t PC = CodeSegment.size();
    int32_t Offset = 0xF0F0;

    int TargetIndex = Instr.InstrT == InstructionType::Bra ? 0 : 1;
    BasicBlock *Target = dynamic_cast<BasicBlock *>(Instr.arguments().at(TargetIndex).R.Ptr);
    if (State.BlockAddresses.find(Target) == State.BlockAddresses.end()) {
      State.Fixups.push_back({Target, PC});
    } else {
      Offset = (State.BlockAddresses[Target] - PC) / 4;
      if (Offset != (int16_t)Offset) {
        throw logic_error("Relative jump out of range.");
      }
    }

    Op BranchOp;
    switch (Instr.InstrT) {
    case InstructionType::Bra:
      BranchOp = Op::BEQ;
      break;
    case InstructionType::Bne:
      BranchOp = Op::BNE;
      break;
    case InstructionType::Beq:
      BranchOp = Op::BEQ;
      break;
    case InstructionType::Ble:
      BranchOp = Op::BLE;
      break;
    case InstructionType::Blt:
      BranchOp = Op::BLT;
      break;
    case InstructionType::Bge:
      BranchOp = Op::BGE;
      break;
    case InstructionType::Bgt:
      BranchOp = Op::BGT;
      break;
    default:
      throw logic_error("This is not a branch instruction.");
    }

    emitF1(BranchOp, CmpReg, 0, Offset);
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

  void init(int32_t StackOffset) {
    emitF1(Op::SUBI, Reg::FP, Reg::GR, StackOffset);
    emitF2(Op::ADD, Reg::SP, Reg::FP, Reg::R0);
    emitF1(Op::JSR, 0, 0, 0);
  }

  void setMainAddress(Function *Main) {
    int32_t MainAddr = FunctionAddresses[Main];
    CodeSegment[8] |= (MainAddr >> 24) & 0x3;
    CodeSegment[9] = (MainAddr >> 16);
    CodeSegment[10] = (MainAddr >> 8);
    CodeSegment[11] = (MainAddr >> 0);
  }

  int mapSpills(Function &F, FunctionAnalyzer &FA,
                unordered_map<RegisterAllocator::VirtualRegister, int32_t> &SpilledRegisterOffsets, int StartOffset) {
    int CurrentOffset = StartOffset;
    for (auto ValueVRPair : *(FA.coloring(&F))) {
      if (FA.isRegisterSpilled(ValueVRPair.second)) {
        CurrentOffset -= 4;
        SpilledRegisterOffsets[ValueVRPair.second] = CurrentOffset;
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
    case InstructionType::Sub:
    case InstructionType::Mul:
    case InstructionType::Div:
    case InstructionType::Cmp:
      emitArithmetic(Instr, State);
      break;
    case InstructionType::Adda:
      // Nothing to emit for Adda. Will fall through to memory instructions.
      break;
    case InstructionType::Load:
    case InstructionType::Store:
      emitMemory(Instr, State);
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
    case InstructionType::Bne:
    case InstructionType::Beq:
    case InstructionType::Ble:
    case InstructionType::Blt:
    case InstructionType::Bge:
    case InstructionType::Bgt:
      emitBranch(Instr, State);
      break;
    case InstructionType::Param: {
      Reg Ra = loadValueIntoRegister(Instr.arguments().at(0), State, Reg::Spill1);
      emitF1(Op::PSH, Ra, SP, -4);
      break;
    }
    case InstructionType::Call: {
      int32_t PC = CodeSegment.size();
      int32_t TargetAddress = FunctionAddresses[dynamic_cast<Function *>(Instr.arguments().at(0).R.Ptr)];
      int16_t Jump = (TargetAddress - PC) / 4;
      emitF1(Op::BSR, 0, 0, Jump);

      // Retrieve return value from stack if there is any.
      auto Coloring = State.FA.coloring(State.CurrentFunction);
      if (Coloring->find(&Instr) != Coloring->end()) {
        Reg Ra = mapValueToRegister(&Instr, State, Reg::Accu);
        emitF1(Op::LDW, Ra, Reg::SP, -4);
        if (Ra == Reg::Accu) {
          storeStackValue(Ra, State.lookupRegisterOffset(&Instr));
        }
      }

      break;
    }
    case InstructionType::Ret: {
      int ParamCount = State.CurrentFunction->parameters().size();
      // Add fake parameter for return value.
      ParamCount = ParamCount == 0 ? 1 : ParamCount;
      int RetValOffset = ParamCount * 4;

      if (Instr.arguments().size() == 1) {
        auto RetVal = Instr.arguments().at(0);
        Reg Ra = loadValueIntoRegister(RetVal, State, Reg::Accu);
        emitF1(Op::STW, Ra, FP, RetValOffset);
      }

      emitF1(Op::ADDI, SP, FP, -36);
      for (uint8_t R = 8; R >= 1; --R) {
        emitF1(Op::POP, R, SP, 4);
      }
      emitF1(Op::POP, FP, SP, 4);
      emitF1(Op::POP, RA, SP, 4);
      emitF1(Op::ADDI, SP, SP, 4 * ParamCount);
      emitF2(Op::RET, 0, 0, RA);
      break;
    }
    case InstructionType::Read: {
      Reg Ra = mapValueToRegister(&Instr, State, Reg::Accu);
      emitF2(Op::RDD, Ra, 0, 0);
      auto Coloring = State.FA.coloring(State.CurrentFunction);
      if (Ra == Reg::Accu && Coloring->find(&Instr) != Coloring->end()) {
        storeStackValue(Ra, State.lookupRegisterOffset(&Instr));
      }
      break;
    }
    case InstructionType::Write: {
      auto Arg = Instr.arguments()[0];
      Reg Rb = loadValueIntoRegister(Arg, State, Reg::Accu);
      emitF2(Op::WRD, 0, Rb, 0);
      break;
    }
    case InstructionType::WriteNL:
      emitF1(Op::WRL, 0, 0, 0);
      break;
    case InstructionType::Kill:
      // Nothing to emit
      break;
    }
  }

  void addFunction(Function *F, FunctionAnalyzer &FA) {
    int32_t Address = static_cast<int32_t>(CodeSegment.size());
    FunctionAddresses[F] = Address;

    // Make space for return value if function has no parameters
    auto &Parameters = F->parameters();
    if (Parameters.empty()) {
      emitF1(Op::ADDI, SP, SP, -4);
    }

    // Prolog
    emitF1(Op::PSH, RA, SP, -4);
    emitF1(Op::PSH, FP, SP, -4);
    emitF1(Op::ADDI, FP, SP, 4);

    // Save registers 1-8
    for (uint8_t R = 1; R <= 8; ++R) {
      emitF1(Op::PSH, R, SP, -4);
    }

    // Create the stack map
    unordered_map<Value *, int32_t> ValueOffsets;

    // Map all parameters
    int Offset = Parameters.size() * 4;
    for (auto Param : F->parameters()) {
      ValueOffsets[Param] = Offset;
      Offset -= 4;
    }

    // Map all arrays
    Offset = -36;
    for (auto &Local : F->locals()) {
      // The second check is not necessary right now, because parameters can only be single word. We still add it to
      // avoid bugs in case arrays or other complex objects can be passed as arguments at some point.
      if (!Local->isSingleWord() && ValueOffsets.find(Local.get()) == ValueOffsets.end()) {
        Offset -= 4 * Local->wordCount();
        ValueOffsets[Local.get()] = Offset;
      }
    }

    unordered_map<RegisterAllocator::VirtualRegister, int32_t> SpilledRegOffsets;
    // Make space for spills
    Offset = mapSpills(*F, FA, SpilledRegOffsets, Offset);
    emitF1(Op::ADDI, SP, FP, Offset);

    // Process all basic blocks
    auto Blocks = linearize(F);

    DLXGenState CurrentState = {move(SpilledRegOffsets), move(ValueOffsets), FA, F};
    for (auto Block : Blocks) {
      CurrentState.BlockAddresses[Block] = CodeSegment.size();
      for (auto &Instr : Block->instructions()) {
        emitInstruction(*Instr, CurrentState);
      }
    }

    // Process all the branch fixups
    for (auto [Target, PC] : CurrentState.Fixups) {
      if (CurrentState.BlockAddresses.find(Target) == CurrentState.BlockAddresses.end()) {
        throw logic_error("Missing basic block address.");
      }
      int32_t FixupAddr = CurrentState.BlockAddresses[Target];
      int32_t RelativeOffset = (FixupAddr - PC) / 4;
      if (RelativeOffset != (int16_t)RelativeOffset) {
        throw logic_error("Relative jump is out of range.");
      }
      CodeSegment[PC + 2] = RelativeOffset >> 8;
      CodeSegment[PC + 3] = RelativeOffset;
    }
    CurrentState.Fixups.clear();
    CurrentState.BlockAddresses.clear();
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
  int32_t StackOffset = DLXO.addGlobals(M.globals());
  DLXO.init(StackOffset);
  DLXO.addFunctions(M.functions(), FA);
  DLXO.setMainAddress(M.functions().back().get());
  return move(DLXO.CodeSegment);
}
