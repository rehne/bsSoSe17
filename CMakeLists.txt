cmake_minimum_required(VERSION 3.7)
project(CLion)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES server.c)
add_executable(CLion ${SOURCE_FILES})