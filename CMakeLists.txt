cmake_minimum_required(VERSION 2.6)
project(glcltest)

set(CMAKE_C_FLAGS "-O3 -march=native -msse4.1 -msse4.2 -mavx -g")
set(CMAKE_CXX_FLAGS "-march=native -msse4.2 -mavx -g")

# find opencl
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/deps/cmake-findopencl/cmake")
find_package(OpenCL)

# configure glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW docs")
set(GLFW_INSTALL OFF CACHE BOOL "Build the GLFW installation target")
add_subdirectory(deps/glfw)

include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${GLFW_SOURCE_DIR}/include"
    "${GLFW_SOURCE_DIR}/deps"
    ${OPENCL_INCLUDE_DIRS}
    "deps/imgui"
)

# glad
set(GLAD "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
         "${GLFW_SOURCE_DIR}/deps/glad.c")

# imgui
add_definitions(-DIMGUI_DISABLE_INCLUDE_IMCONFIG_H)
set(IMGUI
    "${CMAKE_SOURCE_DIR}/deps/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/deps/imgui/imgui_draw.cpp"
)

# glfw
# add_definitions(-DGLFW_INCLUDE_NONE)

add_executable(
    glcltest
    src/main.cc
    src/glfw-imgui.cc
    ${GLFW_LIBRARIES}
    ${GLAD}
    ${IMGUI}
)

target_link_libraries(
    glcltest
    glfw
    ${OPENGL_glu_LIBRARY}
    ${GLFW_LIBRARIES}
    ${OPENCL_LIBRARIES}
)

if(APPLE)
  target_link_libraries(glcltest "-framework OpenGL")
elseif(UNIX)
  target_link_libraries(glcltest GL)
endif()
