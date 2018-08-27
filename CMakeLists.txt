cmake_minimum_required (VERSION 3.2)

SET(CMAKE_CXX_FLAGS "-std=c++17 -fno-devirtualize -o3")

add_executable (fastfunction "test/test.cpp" "include/fastfunction.h")

target_include_directories(fastfunction PUBLIC "include")
