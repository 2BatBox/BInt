cmake_minimum_required(VERSION 3.21)
project(temp)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fvisibility=hidden -s")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fomit-frame-pointer -fno-exceptions -march=native")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

add_executable(temp src/main.cpp
        src/hex.h)
