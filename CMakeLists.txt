cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 17)
project(sch)
enable_testing()
option(SCH_ENABLE_TESTS OFF)
add_subdirectory(test)

add_library(sch INTERFACE)
target_include_directories(sch INTERFACE include)
