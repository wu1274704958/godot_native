cmake_minimum_required(VERSION 3.0.0)
project(init VERSION 0.1.0)

include(CTest)
enable_testing()
include_directories(../godot-cpp/include/core)
include_directories(../godot-cpp/include/gen)
include_directories(../godot-cpp/godot_headers)
include_directories(../godot-cpp/include)

add_library(init init.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
