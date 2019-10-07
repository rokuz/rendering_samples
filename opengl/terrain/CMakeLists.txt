cmake_minimum_required(VERSION 3.13)
project(opengl_terrain)

set(CMAKE_CXX_STANDARD 17)

if (APPLE)
  add_definitions(-Wno-deprecated)
endif()

find_package(OpenGL REQUIRED)

if (WIN32)
  include_directories(../../3party/rf/3party/gl3w)
endif()

configure_file(../../assets/heightmap/heightmap.png heightmap.png COPYONLY)
configure_file(terrain.vsh.glsl terrain.vsh.glsl COPYONLY)
configure_file(terrain.gsh.glsl terrain.gsh.glsl COPYONLY)
configure_file(terrain.fsh.glsl terrain.fsh.glsl COPYONLY)

configure_file(wireframe.vsh.glsl wireframe.vsh.glsl COPYONLY)
configure_file(wireframe.fsh.glsl wireframe.fsh.glsl COPYONLY)
configure_file(wireframe.gsh.glsl wireframe.gsh.glsl COPYONLY)

configure_file(normals.vsh.glsl normals.vsh.glsl COPYONLY)
configure_file(normals.fsh.glsl normals.fsh.glsl COPYONLY)
configure_file(normals.gsh.glsl normals.gsh.glsl COPYONLY)

add_executable(${PROJECT_NAME} opengl_terrain.cpp)

target_link_libraries(${PROJECT_NAME} rf ${OPENGL_LIBRARY} assimp glfw gtest ${PTHREADS})
if (WIN32)
  target_link_libraries(${PROJECT_NAME} gl3w)
endif()
