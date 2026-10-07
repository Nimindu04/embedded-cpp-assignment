cmake_minimum_required(VERSION 3.16)

project(task1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task1_tests
    task1_tests.cpp
)

target_compile_options(task1_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -fno-exceptions
    -fno-rtti
)