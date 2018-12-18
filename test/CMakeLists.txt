add_executable(cs241c-test
  cs241-test.cpp
  DLXVM.cpp
  DLXVM.h)

target_link_libraries(cs241c-test PRIVATE Catch2)
