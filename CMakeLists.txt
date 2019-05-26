cmake_minimum_required(VERSION 3.13)

project(rendering_samples)

set(CMAKE_MACOSX_RPATH OFF)
set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(3party/rf)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/3party/assimp/include)
include_directories(3party/rf/3party/assimp/include)
include_directories(3party/rf/3party/cxxopts/include)
include_directories(3party/rf/3party/glfw/include)
include_directories(3party/rf/3party/glm)
include_directories(3party/rf/3party/glm-aabb)
include_directories(3party/rf/3party/stb)

add_subdirectory(3party/rf)
add_subdirectory(opengl/basic)
