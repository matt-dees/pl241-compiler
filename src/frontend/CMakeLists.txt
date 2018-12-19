add_library(cs241c-frontend
  ast/Expr.cpp
  ast/Expr.h
  ast/Stmt.cpp
  ast/Stmt.h
  Lexer.cpp
  Lexer.h)

target_include_directories(cs241c-frontend INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
