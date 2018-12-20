cmake_minimum_required(VERSION 3.12)
project(cs241c)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(test)
