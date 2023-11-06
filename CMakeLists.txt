cmake_minimum_required(VERSION 3.22)
project(sandboxer_poc C)

find_library(LIBSECCOMP seccomp)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lseccomp")

add_executable(sandboxer_poc main.c cli.c sandbox.c)
target_link_libraries(sandboxer_poc seccomp)
