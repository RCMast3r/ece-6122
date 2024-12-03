cmake_minimum_required(VERSION 3.9)
project(lab3)
find_package(OpenGL REQUIRED)

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME tinyobjloader
  URL https://github.com/tinyobjloader/tinyobjloader/archive/refs/tags/v2.0.0rc13.tar.gz
)

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY  https://github.com/g-truc/glm.git
        GIT_TAG         bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

set(OpenGL_GL_PREFERENCE GLVND)
set(GLFW_BUILD_X11 ON)
set(GLFW_BUILD_WAYLAND OFF)

CPMAddPackage(
  NAME glfw3
  URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
)
CPMAddPackage(
	NAME assimp
	URL https://github.com/assimp/assimp/archive/refs/tags/v5.4.3.tar.gz
)

# generates compile commands for use with vscode / IDEs to enable auto-complete
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILES_COMMANDS ON)


find_package(OpenGL REQUIRED)
# find_package(glm REQUIRED CONFIG)
find_package(GLEW REQUIRED)

add_executable(lab3
	src/chess_3D_view.cpp
	src/shader.cpp
	src/controls.cpp
	src/texture.cpp
	src/objloader.cpp
	src/chessComponent.cpp
)


target_compile_definitions(lab3 PUBLIC USE_LAB3_ASSIMP USE_ASSIMP)

target_include_directories(lab3 PUBLIC include)
target_compile_features(lab3 PUBLIC cxx_std_20)
target_link_libraries(lab3 PUBLIC glm::glm glfw GLEW::GLEW OpenGL::OpenGL assimp::assimp)

