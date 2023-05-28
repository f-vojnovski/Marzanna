
cmake_minimum_required (VERSION 3.8)

project ("Marzanna")

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_executable(Engine engine/main.cpp)

