cmake_minimum_required(VERSION 3.16)

project(task3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task3_tests
    task3_tests.cpp
)

target_compile_options(task3_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -fno-exceptions
    -fno-rtti
)