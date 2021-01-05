cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day21)
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day21 ${SOURCES})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
target_link_libraries(day21  ${CONAN_LIBS})
file(GLOB TEST_SOURCES tests/*.cpp)

