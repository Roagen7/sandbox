cmake_minimum_required(VERSION 3.22)
project(sandboxer_poc C)

set(CMAKE_C_STANDARD 99)

add_executable(sandboxer_poc main.c cli.c sanbox.c)
