cmake_minimum_required(VERSION 2.8)

project(auto-complementation)

set(SOURCE complementer.cpp main.cpp)
set(HEADER complementer.h)

set(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(auto-complementation ${SOURCE})