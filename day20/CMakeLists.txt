cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day20)
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day20 ${SOURCES})
target_link_libraries(day20  ${CONAN_LIBS})
file(GLOB TEST_SOURCES tests/*.cpp)

