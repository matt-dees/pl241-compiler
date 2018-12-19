add_library(cs241c-frontend
  Lexer.cpp
  Lexer.h)

target_include_directories(cs241c-frontend INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
