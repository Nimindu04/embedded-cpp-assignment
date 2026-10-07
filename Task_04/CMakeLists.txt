cmake_minimum_required(VERSION 3.16)

project(task4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task4_virtual
    task4_virtual.cpp
)

add_executable(task4_crtp
    task4_crtp.cpp
)

target_compile_options(task4_virtual PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -fno-exceptions
    -fno-rtti
    -Os
)

target_compile_options(task4_crtp PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -fno-exceptions
    -fno-rtti
    -Os
)