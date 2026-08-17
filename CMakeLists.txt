cmake_minimum_required(VERSION 3.21)
project(TEST VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILE_COMMANDS_DIR "${CMAKE_BINARY_DIR}")
configure_file(${CMAKE_SOURCE_DIR}/.clangd.in ${CMAKE_SOURCE_DIR}/.clangd @ONLY)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        OpenGL::GL
)
