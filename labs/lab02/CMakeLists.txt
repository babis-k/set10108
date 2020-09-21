project(set10108-lab02)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
include_directories(../contrib)

add_executable(thread_pool thread_pool.cpp)
add_executable(pipeline pipeline.cpp)