cmake_minimum_required(VERSION 3.15)
project(OpenGL_Learning)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(plog CONFIG REQUIRED)

add_subdirectory(src/1.getting_started/1.hello_window)