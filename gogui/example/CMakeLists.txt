cmake_minimum_required(VERSION 2.8.4)
project(example)
add_subdirectory(../cppjson ${CMAKE_CURRENT_BINARY_DIR}/cppjson)

SET(CMAKE_CXX_FLAGS "-std=c++11")
include_directories("../cppjson/include")
include_directories("../gogui_core/include")

add_executable(line line.cpp)
target_link_libraries(line cppjson)
