add_subdirectory(codegen)
add_subdirectory(frontend)
add_subdirectory(ir)
add_subdirectory(util)
add_subdirectory(vcg)

add_executable(cs241c cs241c.cpp)
target_link_libraries(cs241c cs241c-frontend cs241c-ir cs241c-util cs241c-vcg)
