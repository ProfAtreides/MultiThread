cmake_minimum_required(VERSION 3.23)
project(List2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Zad1 Zad1/jobShop.cpp)
add_executable(Zad2 Zad2/QAP.cpp)
add_executable(TEST test.cpp)

option(ENABLE_MPI "Enable MPI parallelization" ON)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
set(CMAKE_C_FLAGS "${CMAKE_FLAGS} ${MPI_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_EXE_LINKER_FLAGS}")

target_link_libraries(TEST ${MPI_LIBRARIES})
target_link_libraries(Zad2 ${MPI_LIBRARIES})
target_link_libraries(Zad1 ${MPI_LIBRARIES})

