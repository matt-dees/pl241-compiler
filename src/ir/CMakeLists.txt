add_library(cs241c-ir
  BasicBlock.cpp
  BasicBlock.h
  BasicBlockTerminator.h
  Function.cpp
  Function.h
  Instruction.cpp
  Instruction.h
  Module.cpp
  Module.h
  Value.cpp
  Value.h)

target_include_directories(cs241c-ir INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
