cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day19)
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day19 ${SOURCES})
target_link_libraries(day19  ${CONAN_LIBS})
file(GLOB TEST_SOURCES tests/*.cpp)

