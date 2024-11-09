cmake_minimum_required(VERSION	3.12 FATAL_ERROR)
project(lab4 LANGUAGES CUDA CXX)
# will check for a working cuda compiler 

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

add_executable(lab4 src/lab4.cu src/GameOfLife.cu src/GameOfLifeImplementations.cu)
target_include_directories(lab4 PUBLIC include)
target_link_libraries(lab4 PUBLIC sfml-graphics)
set_target_properties(lab4 PROPERTIES CUDA_ARCHITECTURES all-major)