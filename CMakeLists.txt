cmake_minimum_required(VERSION 3.16)
project(csopesy LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# -------------------------------------------------------------------
# GLFW
# -------------------------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

# -------------------------------------------------------------------
# Dear ImGui (docking branch)
# -------------------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# -------------------------------------------------------------------
# stb (single header library collection)
# -------------------------------------------------------------------
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

# -------------------------------------------------------------------
# Glad (local — generated via glad CLI tool)
# -------------------------------------------------------------------
set(GLAD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/glad)

# -------------------------------------------------------------------
# ImGui sources (added directly)
# -------------------------------------------------------------------
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# -------------------------------------------------------------------
# Application sources
# -------------------------------------------------------------------
file(GLOB_RECURSE CSOPESY_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# -------------------------------------------------------------------
# Executable
# -------------------------------------------------------------------
add_executable(csopesy
    ${CSOPESY_SOURCES}
    ${IMGUI_SOURCES}
    ${GLAD_SOURCE_DIR}/src/glad.c
)

target_include_directories(csopesy PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imgui_SOURCE_DIR}/misc/cpp
    ${stb_SOURCE_DIR}
    ${GLAD_SOURCE_DIR}/include
)

find_package(Threads REQUIRED)

target_link_libraries(csopesy PRIVATE
    glfw
    opengl32
    Threads::Threads
)

# -------------------------------------------------------------------
# Copy configuration file to build directory
# -------------------------------------------------------------------
file(COPY ${CMAKE_SOURCE_DIR}/config.txt DESTINATION ${CMAKE_BINARY_DIR})