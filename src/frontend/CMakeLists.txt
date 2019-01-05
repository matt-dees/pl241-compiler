add_library(cs241c-frontend
  ast/Computation.cpp
  ast/Computation.h
  ast/Decl.cpp
  ast/Decl.h
  ast/Expr.cpp
  ast/Expr.h
  ast/Stmt.cpp
  ast/Stmt.h
  IrGenContext.cpp
  IrGenContext.h
  Lexer.cpp
  Lexer.h
  Parser.cpp
  Parser.h)

target_include_directories(cs241c-frontend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cs241c-frontend cs241c-ir)
