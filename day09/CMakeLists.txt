cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day09)
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day09 ${SOURCES})
