add_executable(cs241c-test
  cs241c-test.cpp
  DLXVM.cpp
  DLXVM.h
  RunTestPrograms.cpp
  TestDomTreeGen.cpp)

target_compile_definitions(cs241c-test PRIVATE CS241C_TEST_PROGRAMS_DIR=\"${CMAKE_SOURCE_DIR}/programs\")
target_link_libraries(cs241c-test Catch2 cs241c-frontend cs241c-util cs241c-vcg cs241c-codegen)
