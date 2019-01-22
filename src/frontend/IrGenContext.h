#ifndef CS241C_FRONTEND_IRGENCONTEXT_H
#define CS241C_FRONTEND_IRGENCONTEXT_H

#include "Instruction.h"
#include "Value.h"
#include "Variable.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cs241c {
class IrGenContext {
  int BasicBlockCounter = 0;
  int InstructionCounter = 0;

  std::unordered_map<std::string, Function *> Functions;
  std::unordered_map<std::string, GlobalVariable *> GlobalVariables;
  std::vector<std::unique_ptr<ConstantValue>> Constants;
  std::unordered_map<std::string, LocalVariable *> LocalVariables;
  std::vector<std::unique_ptr<BasicBlock>> Blocks;

  BasicBlock *CurrentBlock;

public:
  BasicBlock *&currentBlock();
  std::vector<std::unique_ptr<ConstantValue>> &&constants();
  std::vector<std::unique_ptr<BasicBlock>> &&blocks();

  std::string genBasicBlockName();
  int genInstructionId();

  void beginScope();

  void declare(Function *Func);
  void declare(GlobalVariable *Var);
  void declare(LocalVariable *Var);

  Variable *lookupVariable(const std::string &Ident);
  Function *lookupFuncion(const std::string &Ident);

  ConstantValue *makeConstant(int Val);

  template <typename T, typename... Params> T *makeInstruction(Params... Args) {
    auto Instr = std::make_unique<T>(genInstructionId(), Args...);
    T *InstrP = Instr.get();
    CurrentBlock->appendInstruction(move(Instr));
    return InstrP;
  }

  BasicBlock *makeBasicBlock();
};
} // namespace cs241c

#endif
