cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day18)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
file(GLOB SOURCES src/*.cpp)
# add the executable
add_executable(day18 ${SOURCES})
target_link_libraries(day18  ${CONAN_LIBS})

