add_library(cs241c-ir
  BasicBlock.cpp
  BasicBlock.h
  DominatorTree.cpp
  DominatorTree.h
  Function.cpp
  Function.h
  GlobalVariable.cpp
  GlobalVariable.h
  Instruction.cpp
  Instruction.h
  Module.cpp
  Module.h
  Value.cpp
  Value.h)

target_include_directories(cs241c-ir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
