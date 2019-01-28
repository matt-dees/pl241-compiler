cmake_minimum_required(VERSION 3.10)
project(cs241c)

set(CMAKE_CXX_STANDARD 17)

if (UNIX)
  add_compile_options(-Wall -Wextra -g)
endif ()

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(test)
