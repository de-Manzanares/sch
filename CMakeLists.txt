cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(sch LANGUAGES CXX VERSION 0.2.0)

add_library(sch INTERFACE)

target_include_directories(sch INTERFACE include)

enable_testing()

option(SCH_ENABLE_TESTS OFF)

add_subdirectory(test)

