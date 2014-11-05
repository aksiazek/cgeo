cmake_minimum_required(VERSION 2.8.4)
project(example)
add_subdirectory(../gogui/cppjson ${CMAKE_CURRENT_BINARY_DIR}/cppjson)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g")
include_directories("../gogui/cppjson/include")
include_directories("../gogui/gogui_core/include")

add_executable(datasets datasets.cpp)
add_executable(convex_hull convex_hull.cpp)
target_link_libraries(convex_hull cppjson)
