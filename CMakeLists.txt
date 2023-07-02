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

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

add_subdirectory(engine/vendor/glfw)
add_subdirectory(engine/vendor/glm)
add_subdirectory(engine/vendor/tinyobjloader)

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
    engine/vendor/stb
    glm::glm
    "${PROJECT_SOURCE_DIR}"
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(demo PRIVATE
    glfw
    tinyobjloader
    ${Vulkan_LIBRARIES}
)

# Compile shaders

# Find all shader files
file(GLOB_RECURSE SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.glsl")

# Find glslangValidator program
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)

# Iterate over all shader files & add custom command for compiling shaders for each file
foreach(GLSL ${SHADER_SOURCES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  string(REPLACE ".glsl" "" FILE_NAME_WE ${FILE_NAME})

  set(SPIRV "${PROJECT_BINARY_DIR}/assets/shaders/${FILE_NAME_WE}.spv")

  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL}
  )
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(demo Shaders)

# Copy textures to the build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

# Copy geometries to the build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets/models" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")

# TODO: Enable in production only
# add_subdirectory("engine")
# install(TARGETS demo)