cmake_minimum_required(VERSION 3.0.0)
project(compiler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lexer)
add_subdirectory(lab2)
add_subdirectory(parser)
add_subdirectory(common)
add_subdirectory(lab3)
add_subdirectory(compiler)
add_subdirectory(lab4_1)