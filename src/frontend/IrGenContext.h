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
class Function;
class Module;

struct Symbol {
  Variable *Var;
  std::vector<int> Dimensions;
};

class IrGenContext {
  Module *CompilationUnit;

  std::unordered_map<std::string, Function *> FunctionTable;
  Function *CurrentFunction;

  std::unordered_map<std::string, Symbol> GlobalsTable;
  std::unordered_map<std::string, Symbol> LocalsTable;

  BasicBlock *CurrentBlock;

public:
  IrGenContext(Module *CompilationUnit);

  Value *globalBase();

  BasicBlock *&currentBlock();

  const std::unordered_map<std::string, Symbol> &localsTable() const;

  std::string genBasicBlockName();
  int genInstructionId();

  void beginScope();

  void declare(std::unique_ptr<Function> Func);
  void declare(Symbol Sym, std::unique_ptr<GlobalVariable> Var);
  void declare(Symbol Sym);

  Symbol lookupVariable(const std::string &Ident);
  Function *lookupFuncion(const std::string &Ident);

  ConstantValue *makeConstant(int Val);

  Instruction *makeInstruction(InstructionType);
  Instruction *makeInstruction(InstructionType, Value *);
  Instruction *makeInstruction(InstructionType, Value *, Value *);

  BasicBlock *makeBasicBlock();
};
} // namespace cs241c

#endif
