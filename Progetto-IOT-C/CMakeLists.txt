cmake_minimum_required(VERSION 3.27)
project(Progetto_IOT_C)

set(CMAKE_CXX_STANDARD 17)

add_executable(Progetto_IOT_C mainfordebug.cpp
        hardware_io.h
        hardware_io.cpp)
