cmake_minimum_required(VERSION 3.9)

# this cmake is different than the one given to us from the class because 
# this one uses modern tools (that work with cmake versions >3.14) to allow 
# us to pull down libraries without hassle
project(lab5_server LANGUAGES CXX)

# next, im using cpm (cmake package manager) to pull down, pass options to, and make available the SFML library.
# https://github.com/cpm-cmake/CPM.cmake
include(cmake/CPM.cmake)

# generates compile commands for use with vscode / IDEs to enable auto-complete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# to turn off audio due to missing dependency on targets
set(SFML_BUILD_AUDIO OFF)

# better method to pull in upstream dependencies for local compilation / wrapper around fetchcontent feature within cmake
CPMAddPackage(
  NAME SFML
  GITHUB_REPOSITORY SFML/SFML
  GIT_TAG 11b73743c42cf7ecd7c596ba83fdbf1150ffa96c
)

# set(CMAKE_BUILD_TYPE "Release")
add_executable(lab5_server src/main.cpp src/server.cpp)

target_link_libraries(lab5_server PUBLIC sfml-graphics sfml-network)
target_include_directories(lab5_server PUBLIC include)