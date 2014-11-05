cmake_minimum_required(VERSION 2.8.4)
project(cppjson)

include_directories("include")
add_library(cppjson json.cc)