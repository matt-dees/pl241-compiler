add_library(cs241c-codegen
  DLXGen.cpp
  DLXGen.h)

target_include_directories(cs241c-codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cs241c-codegen cs241c-ir)
