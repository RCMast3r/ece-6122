cmake_minimum_required(VERSION 3.9)

# this cmake is different than the one given to us from the class because 
# this one uses modern tools (that work with cmake versions >3.14) to allow 
# us to pull down libraries without hassle
project(lab3 LANGUAGES CXX)

# next, im using cpm (cmake package manager) to pull down, pass options to, and make available the SFML library.
# https://github.com/cpm-cmake/CPM.cmake
include(cmake/CPM.cmake)

# generates compile commands for use with vscode / IDEs to enable auto-complete
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILES_COMMANDS ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED CONFIG)
find_package(GLEW REQUIRED)
# GLFW
find_package(glfw3 REQUIRED CONFIG)
find_package(tinyobjloader REQUIRED CONFIG)
add_executable(lab3 src/test.cpp src/shader.cpp src/controls.cpp src/objloader.cpp src/texture.cpp src/vboindexer.cpp)
target_compile_features(lab3 PUBLIC cxx_std_20)
target_link_libraries(lab3 PUBLIC glfw glm::glm GLEW::GLEW OpenGL::OpenGL tinyobjloader)

target_include_directories(lab3 PUBLIC include)