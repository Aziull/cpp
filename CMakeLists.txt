cmake_minimum_required(VERSION 3.0.0)
project(compiler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

set(HEADERS 
    headers/Helpers.h 
    headers/Lexer.h 
    headers/TokenTables.h)
set(SOURCES 
    Helpers.cpp 
    Lexer.cpp 
    main.cpp 
    TokenTables.cpp)

add_executable(compiler ${HEADERS} ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
