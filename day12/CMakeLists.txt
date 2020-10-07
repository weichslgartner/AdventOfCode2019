cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day12)
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day12 ${SOURCES})
find_package(fmt)
target_link_libraries(day12 ${CONAN_LIBS})
