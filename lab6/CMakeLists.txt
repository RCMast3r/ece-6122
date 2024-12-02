cmake_minimum_required(VERSION 3.9)

project(lab6 LANGUAGES CXX)

find_package(MPI REQUIRED)

add_executable(lab6 src/main.cpp)

target_link_libraries(lab6 PUBLIC MPI::MPI_CXX)