cmake_minimum_required(VERSION 3.14)
# this cmake is different than the one given to us from the class because this one uses the modern target workflow.
project(lab1 LANGUAGES CXX)

# next, im using cpm (cmake package manager) to pull down, pass options to, and make available the SFML library.
# https://github.com/cpm-cmake/CPM.cmake
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME SFML
  GITHUB_REPOSITORY SFML/SFML
  GIT_TAG 11b73743c42cf7ecd7c596ba83fdbf1150ffa96c
)

