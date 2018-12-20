add_library(cs241c-frontend
  ast/Computation.cpp
  ast/Computation.h
  ast/Decl.cpp
  ast/Decl.h
  ast/Expr.cpp
  ast/Expr.h
  ast/Stmt.cpp
  ast/Stmt.h
  Lexer.cpp
  Lexer.h
  Parser.cpp
  Parser.h)

target_include_directories(cs241c-frontend INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
