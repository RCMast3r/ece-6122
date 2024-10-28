cmake_minimum_required(VERSION 3.9)

# this cmake is different than the one given to us from the class because 
# this one uses modern tools (that work with cmake versions >3.14) to allow 
# us to pull down libraries without hassle
project(lab3 LANGUAGES CXX)

# next, im using cpm (cmake package manager) to pull down, pass options to, and make available the SFML library.
# https://github.com/cpm-cmake/CPM.cmake
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME tinyobjloader
  URL https://github.com/tinyobjloader/tinyobjloader/archive/refs/tags/v2.0.0rc13.tar.gz
)

CPMAddPackage(
  NAME glm
  URL https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.zip 
)
set(OpenGL_GL_PREFERENCE GLVND)
CPMAddPackage(
  NAME glfw3
  URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
)

# generates compile commands for use with vscode / IDEs to enable auto-complete
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILES_COMMANDS ON)


find_package(OpenGL REQUIRED)
# find_package(glm REQUIRED CONFIG)
find_package(GLEW REQUIRED)
# GLFW
# find_package(glfw3 REQUIRED CONFIG)
# find_package(tinyobjloader CONFIG)
add_executable(lab3 src/test.cpp 
  src/shader.cpp 
  src/controls.cpp 
  src/objloader.cpp
  src/texture.cpp
  src/vboindexer.cpp
  src/transform.cpp
  )
target_compile_features(lab3 PUBLIC cxx_std_20)
target_link_libraries(lab3 PUBLIC glfw glm GLEW::GLEW OpenGL::OpenGL tinyobjloader)

target_include_directories(lab3 PUBLIC include)