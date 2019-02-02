add_library(cs241c-util
  Filesystem.cpp
  Filesystem.h
  NameGen.h
  NameGen.cpp)

target_include_directories(cs241c-util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
