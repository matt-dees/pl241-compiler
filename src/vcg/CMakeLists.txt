add_library(cs241c-vcg
  VcgGen.cpp
  VcgGen.h
  VcgVisitor.cpp
  VcgVisitor.h )

target_include_directories(cs241c-vcg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cs241c-vcg cs241c-ir)
