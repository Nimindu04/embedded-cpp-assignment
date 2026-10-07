cmake_minimum_required(VERSION 3.16)

project(task5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task5_tests
    task5_tests.cpp
)

target_compile_options(task5_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -fno-exceptions
    -fno-rtti
    -pthread
)

target_link_options(task5_tests PRIVATE
    -pthread
)