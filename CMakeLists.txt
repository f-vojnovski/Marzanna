cmake_minimum_required(VERSION 3.20)

# Precompile headers macro
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

project(MarzannaDemo LANGUAGES CXX)

add_subdirectory(engine/vendor/glfw)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    endif()
    add_compile_definitions(MZ_DEBUG)
else()
    add_compile_definitions(MZ_NODEBUG)
endif()

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

ADD_MSVC_PRECOMPILED_HEADER("precompiled.h" "precompiled.cpp" "engine/src/mzpch.h")

add_executable(demo)
target_sources(demo PRIVATE "demo.cpp")

target_include_directories(demo PRIVATE 
    engine/vendor/spdlog/include 
    engine/vendor/glfw/include
    "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(demo PRIVATE glfw)

add_subdirectory("engine")

install(TARGETS demo)