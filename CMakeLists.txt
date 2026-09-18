cmake_minimum_required(VERSION 3.16)
project(dsl_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(dsl_compiler main.cpp ${SOURCES})
