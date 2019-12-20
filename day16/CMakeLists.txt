cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set the project name
project(day16)
file(GLOB SOURCES src/*.cpp)
# add the executable
set(LSAN_OPTIONS "verbosity=1:log_threads=1")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
add_executable(day16 ${SOURCES})
find_package(fmt)
target_link_libraries(day16 fmt::fmt)