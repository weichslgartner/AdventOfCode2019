cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
# set the project name
project(day18)
# add the executable
add_executable(day18 src/day18.cpp)
target_link_libraries(day18  ${CONAN_LIBS})

