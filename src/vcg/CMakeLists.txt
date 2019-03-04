add_library(cs241c-vcg
  Vcg.cpp
  Vcg.h)

target_include_directories(cs241c-vcg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cs241c-vcg cs241c-util cs241c-ir)
