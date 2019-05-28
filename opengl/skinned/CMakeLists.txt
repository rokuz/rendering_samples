cmake_minimum_required(VERSION 3.13)
project(opengl_skinned)

set(CMAKE_CXX_STANDARD 17)

if (APPLE)
  add_definitions(-Wno-deprecated)
endif()

find_package(OpenGL REQUIRED)

if (WIN32)
  include_directories(../../3party/rf/3party/gl3w)
endif()

configure_files(../../assets/army_pilot army_pilot COPYONLY)
configure_file(../../assets/basic/default.png default.png COPYONLY)
configure_file(skinned.vsh.glsl skinned.vsh.glsl COPYONLY)
configure_file(skinned.fsh.glsl skinned.fsh.glsl COPYONLY)

add_executable(${PROJECT_NAME} opengl_skinned.cpp)

target_link_libraries(${PROJECT_NAME} rf ${OPENGL_LIBRARY} assimp glfw gtest ${PTHREADS})
if (WIN32)
  target_link_libraries(${PROJECT_NAME} gl3w)
endif()
