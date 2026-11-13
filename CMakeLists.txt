cmake_minimum_required(VERSION 3.5)
project(BigInt CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -march=native -g3")

add_executable(autotest autotest.cpp)
add_executable(test_div_by_zero test_div_by_zero.cpp)
