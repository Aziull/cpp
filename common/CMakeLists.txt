project(Common LANGUAGES CXX)

set(INCLUDE_PREFIX "include/Common")
set(HEADERS
    ${INCLUDE_PREFIX}/Common.h
    ${INCLUDE_PREFIX}/Types.h
)

set(INCLUDE_TARGET ${PROJECT_NAME}.Include)
add_library(${INCLUDE_TARGET} INTERFACE ${HEADERS})

target_include_directories(${INCLUDE_TARGET}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set(SOURCE_PREFIX "src")
set(SOURCES
    ${SOURCE_PREFIX}/Common.cpp 
)

set(STATIC_TARGET ${PROJECT_NAME}.Static)
add_library(${STATIC_TARGET} STATIC ${SOURCES})

target_link_libraries(${STATIC_TARGET} PUBLIC ${INCLUDE_TARGET})