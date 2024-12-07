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
	NAME assimp
	URL https://github.com/assimp/assimp/archive/refs/tags/v5.4.3.tar.gz
)

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY  https://github.com/g-truc/glm.git
        GIT_TAG         bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(OpenGL_GL_PREFERENCE GLVND)
set(GLFW_BUILD_X11 ON)
set(GLFW_BUILD_WAYLAND OFF)
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
# add_executable(lab3 src/test.cpp 
#   # src/shader.cpp 
#   src/controls.cpp 
#   src/objloader.cpp
#   src/texture.cpp
#   src/vboindexer.cpp
#   src/transform.cpp

#   src/ogl_utils/shader.cpp

#   src/baseoglmanaging.cpp
#   )

  
# target_compile_features(lab3 PUBLIC cxx_std_20)
# target_link_libraries(lab3 PUBLIC glm::glm glfw GLEW::GLEW OpenGL::OpenGL tinyobjloader assimp::assimp)
# target_include_directories(lab3 PUBLIC include)
# target_include_directories(lab3 PUBLIC include/ogl_utils)

add_executable(tutorial src/tutorial_test.cpp 
  src/controls.cpp 
  src/texture.cpp
  src/vboindexer.cpp
  src/transform.cpp
  
  src/baseoglmanaging.cpp
  src/chessComponent.cpp
  src/chesspiece.cpp
  src/cmdlinehandler.cpp
  src/ogl_utils/shader.cpp
  src/ogl_utils/objloader.cpp
)

  
target_compile_features(tutorial PUBLIC cxx_std_20)
target_link_libraries(tutorial PUBLIC glm::glm glfw GLEW::GLEW OpenGL::OpenGL assimp::assimp tinyobjloader)
target_include_directories(tutorial PUBLIC include)
target_include_directories(tutorial PUBLIC include/ogl_utils)
