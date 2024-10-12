cmake_minimum_required(VERSION 3.9)

# this cmake is different than the one given to us from the class because 
# this one uses modern tools (that work with cmake versions >3.14) to allow 
# us to pull down libraries without hassle
project(lab2 LANGUAGES CXX)

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

find_package(OpenMP REQUIRED)


add_executable(samplecode src/SampleCode.cpp)

target_link_libraries(samplecode PUBLIC sfml-graphics)


add_executable(lab2 src/main.cpp)
target_compile_features(lab2 PUBLIC cxx_std_20)
target_link_libraries(lab2 PUBLIC sfml-graphics OpenMP::OpenMP_CXX)
target_include_directories(lab2 PUBLIC include)


add_executable(mptest src/openmptesting.cpp)
target_link_libraries(mptest PUBLIC OpenMP::OpenMP_CXX)