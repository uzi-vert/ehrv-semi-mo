cmake_minimum_required(VERSION 3.16)

project(csopesy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE CSOPESY_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(csopesy
    ${CSOPESY_SOURCES}
)

target_include_directories(csopesy PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

find_package(Threads REQUIRED)

target_link_libraries(csopesy PRIVATE
    Threads::Threads
)