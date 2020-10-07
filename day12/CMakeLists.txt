cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
# set the project name
project(day12)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day12 ${SOURCES})
find_package(fmt)
target_link_libraries(day12 ${CONAN_LIBS})
