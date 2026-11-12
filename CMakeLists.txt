cmake_minimum_required(VERSION 2.8)
project(BigInt)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -march=native -O3")

add_executable(autotest autotest.cpp)
