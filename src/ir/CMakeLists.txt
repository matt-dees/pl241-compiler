add_library(cs241c-ir
  BasicBlock.cpp
  BasicBlock.h
  CommonSubexElimPass.cpp
  CommonSubexElimPass.h
  ConstExprEvalPass.cpp
  ConstExprEvalPass.h
  DeadCodeEliminationPass.cpp
  DeadCodeEliminationPass.h
  DominatorTree.cpp
  DominatorTree.h
  Function.cpp
  Function.h
  Instruction.cpp
  Instruction.h
  IntegrityCheckPass.cpp
  IntegrityCheckPass.h
  IrGenContext.cpp
  IrGenContext.h
  Mem2VarPass.cpp
  Mem2VarPass.h
  Module.cpp
  Module.h
  Pass.h
  RegisterAllocation.cpp
  RegisterAllocation.h
  SSAContext.cpp
  SSAContext.h
  Value.cpp
  Value.h
  Variable.cpp
  Variable.h)

target_include_directories(cs241c-ir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cs241c-ir PRIVATE cs241c-util PUBLIC cs241c-vcg)
