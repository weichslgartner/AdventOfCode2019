cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
file(GLOB TEST_SOURCES *.cpp)
# add the executable
project(test18)
add_executable(test18 ${TEST_SOURCES})
target_link_libraries(test18 ${CONAN_LIBS})
add_test(NAME example_test COMMAND test18)

