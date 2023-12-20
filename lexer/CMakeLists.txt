project(Lexer LANGUAGES CXX)

set(INCLUDE_PREFIX "include/Lexer")
set(HEADERS
    ${INCLUDE_PREFIX}/Helpers.h 
    ${INCLUDE_PREFIX}/Lexer.h 
    ${INCLUDE_PREFIX}/TokenTables.h
)

set(INCLUDE_TARGET ${PROJECT_NAME}.Include)
add_library(${INCLUDE_TARGET} INTERFACE ${HEADERS})

target_include_directories(${INCLUDE_TARGET}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set(SOURCE_PREFIX "src")
set(SOURCES
    ${SOURCE_PREFIX}/Helpers.cpp 
    ${SOURCE_PREFIX}/Lexer.cpp 
    ${SOURCE_PREFIX}/TokenTables.cpp
)

set(STATIC_TARGET ${PROJECT_NAME}.Static)
add_library(${STATIC_TARGET} STATIC ${SOURCES})

target_link_libraries(${STATIC_TARGET} PUBLIC ${INCLUDE_TARGET})