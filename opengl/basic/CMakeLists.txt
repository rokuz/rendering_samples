cmake_minimum_required(VERSION 3.13)
project(opengl_basic)

set(CMAKE_CXX_STANDARD 17)

if (APPLE)
  add_definitions(-Wno-deprecated)
endif()

find_package(OpenGL REQUIRED)

if (WIN32)
  include_directories(../../3party/rf/3party/gl3w)
endif()

configure_file(lambert.vsh.glsl lambert.vsh.glsl COPYONLY)
configure_file(lambert.fsh.glsl lambert.fsh.glsl COPYONLY)

add_executable(${PROJECT_NAME} opengl_basic.cpp)

target_link_libraries(${PROJECT_NAME} rf ${OPENGL_LIBRARY} assimp glfw gtest ${PTHREADS})
if (WIN32)
  target_link_libraries(${PROJECT_NAME} gl3w)
endif()
