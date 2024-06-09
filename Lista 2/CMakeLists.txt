cmake_minimum_required(VERSION 3.23)
project(MultiThread)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Zad1 Zad1/main.cpp)

find_package(MPI REQUIRED)

target_link_libraries(Zad1 PUBLIC MPI::MPI_CXX)