#ifndef CS241C_FRONTEND_IRGENCONTEXT_H
#define CS241C_FRONTEND_IRGENCONTEXT_H

#include "BasicBlock.h"
#include "Instruction.h"
#include "Value.h"
#include "Variable.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cs241c {
class Module;

struct Symbol {
  Variable *Var;
  std::vector<int> Dimensions;
};

class IrGenContext {
  Module *CompilationUnit;

  std::unordered_map<std::string, Function *> FunctionTable;
  std::unordered_map<std::string, Symbol> GlobalsTable;
  std::vector<std::unique_ptr<ConstantValue>> Constants;
  std::unordered_map<std::string, Symbol> LocalsTable;
  std::vector<std::unique_ptr<BasicBlock>> Blocks;

  BasicBlock *CurrentBlock;

private:
  SSAContext nodeToSSA(BasicBlock *CurrentBB, SSAContext SSACtx);
  void propagateChangeToPhis(BasicBlock *SourceBB, Variable *ChangedVar, Value *NewVal);
  void genAllPhiInstructions(BasicBlock *CurrentBB);

public:
  IrGenContext(Module *CompilationUnit);

  Value *globalBase();

  BasicBlock *&currentBlock();
  std::vector<std::unique_ptr<ConstantValue>> &&constants();
  std::vector<std::unique_ptr<BasicBlock>> &&blocks();

  std::string genBasicBlockName();
  int genInstructionId();

  void beginScope();

  void declare(std::unique_ptr<Function> Func);
  void declare(Symbol Sym, std::unique_ptr<GlobalVariable> Var);
  void declare(Symbol Sym);

  Symbol lookupVariable(const std::string &Ident);
  Function *lookupFuncion(const std::string &Ident);

  ConstantValue *makeConstant(int Val);

  template <typename T, typename... Params> T *makeInstruction(Params... Args) {
    auto Instr = std::make_unique<T>(genInstructionId(), Args...);
    T *InstrP = Instr.get();
    CurrentBlock->appendInstruction(move(Instr));
    return InstrP;
  }

  BasicBlock *makeBasicBlock();

  void compUnitToSSA();
};
} // namespace cs241c

#endif
