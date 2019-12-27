cmake_minimum_required(VERSION 3.10)
#set(CMAKE_C_COMPILER /usr/bin/gcc-9)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-9)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day15)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day15 ${SOURCES})
