cmake_minimum_required(VERSION 3.20)
project(MarzannaDemo LANGUAGES CXX)

add_executable(demo)
target_sources(demo PRIVATE "demo.cpp")
target_include_directories(demo PRIVATE engine/vendor/spdlog/include "${PROJECT_SOURCE_DIR}")
add_subdirectory("engine")
install(TARGETS demo)

