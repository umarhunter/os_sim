cmake_minimum_required(VERSION 3.22)
project(opsys_project)

set(CMAKE_CXX_STANDARD 23)

add_executable(opsys_project
        main.cpp
        OpSys.cpp
        OpSys.h
        PCB.cpp
        PCB.h
        utility.cpp)
