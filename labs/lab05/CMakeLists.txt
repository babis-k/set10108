project(set10108-lab05)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)
include_directories(../contrib)

add_executable(simd128 simd128.cpp)
add_executable(simd-add simd-add.cpp)
add_executable(simd-normal simd-normal.cpp)