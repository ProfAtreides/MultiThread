cmake_minimum_required(VERSION 3.23)
project(MultiThread)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(Zad1 Zad1/main.cpp)

add_executable(Zad2 Zad2/lab2.cpp)

add_executable(Zad3 Zad3/lab3.cpp)