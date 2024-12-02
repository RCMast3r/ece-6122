cmake_minimum_required(VERSION 3.9)

# this cmake is different than the one given to us from the class because 
# this one uses modern tools (that work with cmake versions >3.14) to allow 
# us to pull down libraries without hassle
project(lab6 LANGUAGES CXX)

find_package(MPI REQUIRED)

# next, im using cpm (cmake package manager) to pull down, pass options to, and make available the SFML library.
# https://github.com/cpm-cmake/CPM.cmake

# generates compile commands for use with vscode / IDEs to enable auto-complete

# set(CMAKE_BUILD_TYPE "Release")
add_executable(lab6 src/main.cpp)

target_link_libraries(lab6 PUBLIC MPI::MPI_CXX)