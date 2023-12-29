project(Parser LANGUAGES CXX)

set(INCLUDE_PREFIX "include/Parser")
set(HEADERS
    ${INCLUDE_PREFIX}/Parser.h 
)

set(INCLUDE_TARGET ${PROJECT_NAME}.Include)
add_library(${INCLUDE_TARGET} INTERFACE ${HEADERS})

target_include_directories(${INCLUDE_TARGET}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set(SOURCE_PREFIX "src")
set(SOURCES
    ${SOURCE_PREFIX}/Parser.cpp
)

set(STATIC_TARGET ${PROJECT_NAME}.Static)
add_library(${STATIC_TARGET} STATIC ${SOURCES})

target_link_libraries(${STATIC_TARGET} PUBLIC ${INCLUDE_TARGET} PRIVATE Common.Static)